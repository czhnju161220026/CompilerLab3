#include "intermediate.h"
#include "grammarTree.h"
#include "hashset.h"
#include "semantic.h"
#include "utils.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *translateExp(Morpheme *exp, HashSet *symTable, char *place)
{
    //printf("Translating Exp\n");
    if (exp == NULL)
    {
        printf("\033[31mThis exp is NULL\n\033[0m");
        return NULL;
    }
    if (exp->type != _Exp)
    {
        printf("\033[31mThis exp is not _EXP\n\033[0m");
        return NULL;
    }
    Morpheme *c = exp->child;
    if (c == NULL)
    {
        printf("\033[31mThis child is NULL\n\033[0m");
        return NULL;
    }
    //EXP -> INT
    if (c->type == _INT)
    {
        //进行一个小优化，将place直接设置为#val
        int value = c->intValue;
        /*char str[256];
        sprintf(str, "%s := #%d\n", place, value);
        char *code = (char *)malloc(strlen(str) + 1);
        strcpy(code, str);
        return code;*/
        sprintf(place, "#%d", value);
        char *code = (char *)malloc(1);
        code[0] = '\0';
        return code;
    }
    // EXP -> ID
    else if (c->type == _ID && c->siblings == NULL)
    {
        //进行一个小优化，将place直接设置为variable， code为空
        Symbol *s = get(symTable, c->idName);
        char *variable = s->variable;
        //char str[256];
        //sprintf(str, "%s := %s\n", place, variable);
        //char *code = (char *)malloc(strlen(str) + 1);
        //strcpy(code, str);
        sprintf(place, "%s", variable);
        char *code = (char *)malloc(1);
        code[0] = '\0';
        return code;
    }
    //EXP -> Exp assignop Exp
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        if (c->child->type == _ID && c->child->siblings == NULL)
        {
            Symbol *s = get(symTable, c->child->idName);
            char *variable = s->variable;
            char *t1 = getTemp();
            char *code1 = translateExp(c->siblings->siblings, symTable, t1);
            char str[256];
            sprintf(str, "%s := %s\n%s := %s\n", variable, t1, place, variable);
            char *code2 = (char *)malloc(strlen(str) + 1);
            strcpy(code2, str);
            return concat(2, code1, code2);
        }
        else
        {
            char *t1 = getTemp();
            char *t2 = getTemp();
            char *code1 = translateExp(c, symTable, t1);
            char *code2 = translateExp(c->siblings->siblings, symTable, t2);
            char str[256];
            sprintf(str, "%s := %s\n%s := %s\n", t1, t2, place, t2);
            char *code3 = (char *)malloc(strlen(str) + 1);
            strcpy(code3, str);
            return concat(3, code1, code2, code3);
        }

        //TODO : array and struct
    }
    //Exp -> LP Exp RP
    else if (c->type == _LP && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {
        return translateExp(c->siblings, symTable, place);
    }
    //Exp -> Exp op Exp
    else if (c->type == _Exp && c->siblings != NULL && (c->siblings->type == _PLUS || c->siblings->type == _MINUS || c->siblings->type == _STAR || c->siblings->type == _DIV) && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        char *t1 = getTemp();
        char *t2 = getTemp();
        char *code1 = translateExp(c, symTable, t1);
        char *code2 = translateExp(c->siblings->siblings, symTable, t2);
        char str[256];
        if (c->siblings->type == _PLUS)
        {
            sprintf(str, "%s := %s + %s\n", place, t1, t2);
        }
        else if (c->siblings->type == _MINUS)
        {
            sprintf(str, "%s := %s - %s\n", place, t1, t2);
        }
        else if (c->siblings->type == _STAR)
        {
            sprintf(str, "%s := %s * %s\n", place, t1, t2);
        }
        else if (c->siblings->type == _DIV)
        {
            sprintf(str, "%s := %s / %s\n", place, t1, t2);
        }
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code2, code3);
    }
    //Exp -> Minus Exp
    else if (c->type == _MINUS && c->siblings != NULL && c->siblings->type == _Exp)
    {
        char *t1 = getTemp();
        char *code1 = translateExp(c->siblings, symTable, t1);
        char str[256];
        sprintf(str, "%s := #0 - %s\n", place, t1);
        char *code2 = (char *)malloc(strlen(str) + 1);
        strcpy(code2, str);
        return concat(2, code1, code2);
    }
    //Exp->Exp relop Exp
    else if ((c->type == _Exp && c->siblings != NULL && (c->siblings->type == _RELOP || c->siblings->type == _AND || c->siblings->type == _OR) && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp) || (c->type == _NOT && c->siblings != NULL && c->siblings->type == _Exp))
    {
        char *label1 = getLabel();
        char *label2 = getLabel();
        char str[256];
        sprintf(str, "%s := #0\n", place);
        char *code0 = (char *)malloc(strlen(str) + 1);
        strcpy(code0, str);

        char *code1 = translateCond(exp, label1, label2, symTable);
        sprintf(str, "LABEL %s :\n%s := #1\n", label1, place);
        char *code2 = (char *)malloc(strlen(str) + 1);
        strcpy(code2, str);

        sprintf(str, "LABEL %s :\n", label2);
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);

        return concat(4, code0, code1, code2, code3);
    }
    // Exp -> Exp()
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {
        if (strcmp(c->idName, "read") == 0)
        {
            char str[256];
            sprintf(str, "READ %s\n", place);
            char *code = (char *)malloc(strlen(str) + 1);
            strcpy(code, str);
            return code;
        }
        else
        {
            char str[256];
            sprintf(str, "%s := CALL %s\n", place, c->idName);
            char *code = (char *)malloc(strlen(str) + 1);
            strcpy(code, str);
            return code;
        }
    }
    // Exp -> Exp (args)
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Args && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP)
    {
        Argument *arglist = NULL;
        char *code0 = translateArgs(c->siblings->siblings, symTable, &arglist);
        //case WRITE arg
        if (strcmp(c->idName, "write") == 0)
        {
            char *argName = arglist->name;
            char *code = concat(4, code0, "WRITE ", argName, "\n");
            return code;
        }
        //other case
        else
        {
            char *code1 = "";
            for (Argument *p = arglist; p != NULL; p = p->next)
            {
                char *argName = p->name;
                code1 = concat(4, code1, "ARG ", argName, "\n");
            }
            char *code = concat(6, code0, code1, place, " := CALL ", c->idName, "\n");
            return code;
        }
    }
    // Exp -> Exp [Exp]
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _LB && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RB && c->siblings->siblings->siblings->siblings == NULL)
    {
        char *t1 = getTemp(); //addr
        char *t2 = getTemp(); //offset
        char *t3 = getTemp(); //4 * offset
        char *t4 = getTemp(); //final addr
        char *code1 = translateExp(c, symTable, t1);
        char *code2 = translateExp(c->siblings->siblings, symTable, t2);
        char str[256];
        sprintf(str, "%s := #4 * %s\n%s := %s + %s\n%s := %s\n", t3, t2, t4, t1, t3, place, t4);
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);
        char *temp = (char *)malloc(strlen(place) + 1);
        strcpy(temp, place);
        sprintf(place, "*%s", temp);
        return concat(3, code1, code2, code3);
    }
    // Exp -> Exp.id
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _DOT && c->siblings->siblings != NULL && c->siblings->siblings->type == _ID && c->siblings->siblings->siblings == NULL)
    {
        char *t1 = getTemp();                        //addr
        char *code1 = translateExp(c, symTable, t1); //Get addr
        Morpheme *fieldId = c->siblings->siblings;
        Symbol *fieldSymbol = get(symTable, fieldId->idName);
        //这种情况要给place之前加上星号
        if (fieldSymbol->symbol_type == INT_SYMBOL || fieldSymbol->symbol_type == FLOAT_SYMBOL)
        {
            char *finalAddr = getTemp();
            int offset = calcFieldOffset(fieldSymbol->name);
            char str[256];
            sprintf(str, "%s := #%d + %s\n%s := %s\n", finalAddr, offset, t1, place, finalAddr);
            char *code2 = (char *)malloc(strlen(str) + 1);
            strcpy(code2, str);
            char *code = concat(2, code1, code2);
            char *temp = (char *)malloc(strlen(place) + 1);
            strcpy(temp, place);
            sprintf(place, "*%s", temp);
            return code;
        }
        //这种情况直接把地址返回给place就可以
        else if (fieldSymbol->symbol_type == ARRAY_SYMBOL || fieldSymbol->symbol_type == STRUCT_VAL_SYMBOL)
        {
            char *finalAddr = getTemp();
            int offset = calcFieldOffset(fieldSymbol->name);
            char str[256];
            sprintf(str, "%s := #%d + %s\n%s := %s\n", finalAddr, offset, t1, place, finalAddr);
            char *code2 = (char *)malloc(strlen(str) + 1);
            strcpy(code2, str);
            char *code = concat(2, code1, code2);
            return code;
        }
    }

    return NULL;
}
//移除该优化，如果出现x+f(x)，f(x)将不被执行，这是错误的
//如果Exp是Exp Semi的形式，那么只有赋值和函数调用是有意义的，其他的Exp可以不翻译，直接返回空串

char* translateExpLikeExpSemi(Morpheme* exp, HashSet* symTable) {
    if (exp == NULL)
    {
        printf("\033[31mThis exp is NULL\n\033[0m");
        return NULL;
    }
    if (exp->type != _Exp)
    {
        printf("\033[31mThis exp is not _EXP\n\033[0m");
        return NULL;
    }
    Morpheme *c = exp->child;
    if (c == NULL)
    {
        printf("\033[31mThis child is NULL\n\033[0m");
        return NULL;
    }
    //EXP -> Exp assignop Exp
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        if (c->child->type == _ID && c->child->siblings == NULL)
        {
            Symbol *s = get(symTable, c->child->idName);
            char *variable = s->variable;
            char *t1 = getTemp();
            char *code1 = translateExp(c->siblings->siblings, symTable, t1);
            char str[256];
            sprintf(str, "%s := %s\n", variable, t1);
            char *code2 = (char *)malloc(strlen(str) + 1);
            strcpy(code2, str);
            return concat(2, code1, code2);
        }
        else
        {
            char *t1 = getTemp();
            char *t2 = getTemp();
            char *code1 = translateExp(c, symTable, t1);
            char *code2 = translateExp(c->siblings->siblings, symTable, t2);
            char str[256];
            sprintf(str, "%s := %s\n", t1, t2);
            char *code3 = (char *)malloc(strlen(str) + 1);
            strcpy(code3, str);
            return concat(3, code1, code2, code3);
        }
    }
    //Exp -> LP Exp RP
    else if (c->type == _LP && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {
        return translateExpLikeExpSemi(c->siblings, symTable);
    }
    // Exp -> Exp()
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {
        if (strcmp(c->idName, "read") == 0) //对于read的忽略返回值的调用是可以忽略的
        {
            char* code = (char*)malloc(1);
            code[0] = '\0';
            return code;
        }
        else
        {
            char* temp = getTemp();
            char str[256];
            sprintf(str, "%s := CALL %s\n", temp, c->idName);
            char *code = (char *)malloc(strlen(str) + 1);
            strcpy(code, str);
            return code;
        }
    }
    //Exp -> Exp(Args)
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Args && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP)
    {
        Argument *arglist = NULL;
        char *code0 = translateArgs(c->siblings->siblings, symTable, &arglist);
        //case WRITE arg
        if (strcmp(c->idName, "write") == 0)
        {
            char *argName = arglist->name;
            char *code = concat(4, code0, "WRITE ", argName, "\n");
            return code;
        }
        //other case
        else
        {
            char *code1 = "";
            for (Argument *p = arglist; p != NULL; p = p->next)
            {
                char *argName = p->name;
                code1 = concat(4, code1, "ARG ", argName, "\n");
            }
            char* temp = getTemp();
            char *code = concat(6, code0, code1, temp, " := CALL ", c->idName, "\n");
            return code;
        }
    }

}

char *translateStmt(Morpheme *stmt, HashSet *symTable)
{
    //printf("Translating Stmt\n");
    if (stmt == NULL || stmt->type != _Stmt)
    {
        printf("\033[31mNot a stmt\n\033[0m");
        return NULL;
    }
    //TODO: 目前只实现了Stmt -> Exp Semi
    Morpheme *c = stmt->child;
    //Stmt -> Exp Semi
    if (c != NULL && c->type == _Exp && c->siblings != NULL && c->siblings->type == _SEMI)
    {
        //printf("Stmt -> Exp ;\n");
        char *t1 = getTemp();
        char *code = translateExp(c, symTable, t1);
        //char* code = translateExpLikeExpSemi(c,symTable);
        return code;
    }
    else if (c->type == _CompSt && c->siblings == NULL)
    {
        return translateCompSt(c, symTable);
    }
    else if (c->type == _RETURN && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI)
    {
        char *t1 = getTemp();
        char *code1 = translateExp(c->siblings, symTable, t1);
        char str[256];
        sprintf(str, "RETURN %s\n", t1);
        char *code2 = (char *)malloc(strlen(str) + 1);
        strcpy(code2, str);
        return concat(2, code1, code2);
    }
    else if (c->type == _IF && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt && c->siblings->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->siblings->type == _ELSE && c->siblings->siblings->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->siblings->siblings->type == _Stmt)
    {
        char *label1 = getLabel();
        char *label2 = getLabel();
        char *label3 = getLabel();
        char *code1 = translateCond(c->siblings->siblings, label1, label2, symTable);
        char *code2 = translateStmt(c->siblings->siblings->siblings->siblings, symTable);
        char *code3 = translateStmt(c->siblings->siblings->siblings->siblings->siblings->siblings, symTable);
        char str[256];
        sprintf(str, "LABEL %s :\n", label1);
        char *code11 = (char *)malloc(strlen(str) + 1);
        strcpy(code11, str);
        sprintf(str, "GOTO %s\n", label3);
        char *code22 = (char *)malloc(strlen(str) + 1);
        strcpy(code22, str);
        sprintf(str, "LABEL %s :\n", label2);
        char *code33 = (char *)malloc(strlen(str) + 1);
        strcpy(code33, str);
        sprintf(str, "LABEL %s :\n", label3);
        char *code44 = (char *)malloc(strlen(str) + 1);
        strcpy(code44, str);
        return concat(7, code1, code11, code2, code22, code33, code3, code44);
    }
    else if (c->type == _IF && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt && c->siblings->siblings->siblings->siblings->siblings == NULL)
    {
        char *label1 = getLabel();
        char *label2 = getLabel();
        char *code1 = translateCond(c->siblings->siblings, label1, label2, symTable);
        char *code2 = translateStmt(c->siblings->siblings->siblings->siblings, symTable);
        char str[256];
        sprintf(str, "LABEL %s :\n", label1);
        char *code11 = (char *)malloc(strlen(str) + 1);
        strcpy(code11, str);
        sprintf(str, "LABEL %s :\n", label2);
        char *code22 = (char *)malloc(strlen(str) + 1);
        strcpy(code22, str);
        return concat(4, code1, code11, code2, code22);
    }
    else if (c->type == _WHILE && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt)
    {
        char *label1 = getLabel();
        char *label2 = getLabel();
        char *label3 = getLabel();
        char *code1 = translateCond(c->siblings->siblings, label2, label3, symTable);
        char *code2 = translateStmt(c->siblings->siblings->siblings->siblings, symTable);
        char str[256];
        sprintf(str, "LABEL %s :\n", label1);
        char *code11 = (char *)malloc(strlen(str) + 1);
        strcpy(code11, str);
        sprintf(str, "LABEL %s :\n", label2);
        char *code22 = (char *)malloc(strlen(str) + 1);
        strcpy(code22, str);
        sprintf(str, "GOTO %s\n", label1);
        char *code33 = (char *)malloc(strlen(str) + 1);
        strcpy(code33, str);
        sprintf(str, "LABEL %s :\n", label3);
        char *code44 = (char *)malloc(strlen(str) + 1);
        strcpy(code44, str);
        return concat(6, code11, code1, code22, code2, code33, code44);
    }
    else
    {
        return NULL;
    }
    return NULL;
}

char *translateCond(Morpheme *exp, char *label_true, char *label_false, HashSet *symTable)
{
    if (exp == NULL)
    {
        printf("\033[31mThis exp is NULL\n\033[0m");
        return NULL;
    }
    if (exp->type != _Exp)
    {
        printf("\033[31mThis exp is not _EXP\n\033[0m");
        return NULL;
    }
    Morpheme *c = exp->child;
    if (c == NULL)
    {
        printf("\033[31mThis child is NULL\n\033[0m");
        return NULL;
    }

    if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _RELOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        char *t1 = getTemp();
        char *t2 = getTemp();
        char *code1 = translateExp(c, symTable, t1);
        char *code2 = translateExp(c->siblings->siblings, symTable, t2);
        char *op = c->siblings->idName;
        char str[256];
        sprintf(str, "IF %s %s %s GOTO %s\nGOTO %s\n", t1, op, t2, label_true, label_false);
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code2, code3);
    }
    else if (c->type == _NOT && c->siblings != NULL && c->siblings->type == _Exp)
    {
        return translateCond(c->siblings, label_false, label_true, symTable);
    }
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _AND && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        char *label1 = getLabel();
        char *code1 = translateCond(c, label1, label_false, symTable);
        char *code2 = translateCond(c->siblings->siblings, label_true, label_false, symTable);
        char str[256];
        sprintf(str, "LABEL %s :\n", label1);
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code3, code2);
    }
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _OR && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        char *label1 = getLabel();
        char *code1 = translateCond(c, label_true, label1, symTable);
        char *code2 = translateCond(c->siblings->siblings, label_true, label_false, symTable);
        char str[256];
        sprintf(str, "LABEL %s :\n", label1);
        char *code3 = (char *)malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code3, code2);
    }
    else
    {
        char *t1 = getTemp();
        char *code1 = translateExp(exp, symTable, t1);
        char str[256];
        sprintf(str, "IF %s != #0 GOTO %s\nGOTO %s\n", t1, label_true, label_false);
        char *code2 = (char *)malloc(strlen(str) + 1);
        strcpy(code2, str);
        return concat(2, code1, code2);
    }
}
//TODO: implement translate args
//Attention: The third parameter 'arglist' stores the temp name for the args.
char *translateArgs(Morpheme *args, HashSet *symTable, Argument **arglist)
{
    if (args == NULL || args->type != _Args)
    {
        printf("\033[31mArgs error\n\033[0m");
        return NULL;
    }
    Morpheme *c = args->child;
    if (c == NULL)
    {
        printf("\033[31mArgs child is NULL\n\033[0m");
        return NULL;
    }
    // Args -> Exp
    else if (c->type == _Exp && c->siblings == NULL)
    {
        char *t1 = getTemp();
        char *code1 = translateExp(c, symTable, t1);
        Argument *arg = (Argument *)malloc(sizeof(Argument));
        arg->name = t1;
        arg->next = *arglist;
        *arglist = arg;
        return code1;
    }
    // Args -> Exp COMMA Args
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _Args && c->siblings->siblings->siblings == NULL)
    {
        char *t1 = getTemp();
        char *code1 = translateExp(c, symTable, t1);
        Argument *arg = (Argument *)malloc(sizeof(Argument));
        arg->name = t1;
        arg->next = *arglist;
        *arglist = arg;
        char *code2 = translateArgs(c->siblings->siblings, symTable, arglist);
        char *code = concat(2, code2, code1);
        return code;
    }
    printf("\033[31mInvalid Args node.\n\033[0m");
    return NULL;
}

char *translateProgram(Morpheme *prog, HashSet *symtable)
{
    //printf("Translating program\n");
    if (prog == NULL || prog->type != _Program)
    {
        printf("\033[31mInvalid program node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = prog->child;
    char *code = translateExtDefList(c, symtable);
    return code;
}
char *translateExtDefList(Morpheme *extDefList, HashSet *symtable)
{
    //printf("Translating ExtDefList\n");
    if (extDefList == NULL || extDefList->type != _ExtDefList)
    {
        printf("\033[31mBad extDefList.\n\033[0m");
        return NULL;
    }
    Morpheme *c = extDefList->child;
    // ExtDefList -> empty
    if (c->type == _BLANK)
    {
        return NULL;
    }
    // ExtDefList -> ExtDef ExtDefList
    else if (c->type == _ExtDef && c->siblings != NULL && c->siblings->type == _ExtDefList && c->siblings->siblings == NULL)
    {
        char *code1, *code2;
        code1 = translateExtDef(c, symtable);
        code2 = translateExtDefList(c->siblings, symtable);
        char *code = concat(2, code1, code2);
        return code;
    }
    printf("\033[31mInvalid extDefList node.\n\033[0m");
    return NULL;
}
char *translateExtDef(Morpheme *extDef, HashSet *symTable)
{
    //printf("Translating ExtDef\n");
    if (extDef == NULL || extDef->type != _ExtDef)
    {
        printf("\033[31mBad ExtDef node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = extDef->child;
    // Becase there is no global variable in this lab,
    // We only need to translate case: ExtDef -> Specifier FunDec Compst
    if (c != NULL && c->type == _Specifier && c->siblings != NULL && c->siblings->type == _FunDec && c->siblings->siblings != NULL && c->siblings->siblings->type == _CompSt && c->siblings->siblings->siblings == NULL)
    {
        char *code;
        char *code1 = translateFunDec(c->siblings, symTable);
        char *code2 = translateCompSt(c->siblings->siblings, symTable);
        code = concat(2, code1, code2);
        return code;
    }
    else
    {
        //We do not care about other cases.
        return NULL;
    }
}
char *translateFunDec(Morpheme *funDec, HashSet *symTable)
{
    //printf("Translating FunDec\n");
    if (funDec == NULL || funDec->type != _FunDec)
    {
        printf("\033[31mBad FunDec node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = funDec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty FunDec node.\n\033[0m");
        return NULL;
    }
    // FunDec -> ID ()
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {

        char *code;
        code = concat(3, "FUNCTION ", c->idName, " :\n");
        return code;
    }
    // FunDec -> ID (VarList)
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _VarList && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP)
    {
        char *code;
        char *code1 = concat(3, "FUNCTION ", c->idName, " :\n");
        char *code2 = translateVarList(c->siblings->siblings, symTable);
        code = concat(2, code1, code2);
        return code;
    }

    printf("\033[31mBad FunDec node.\n\033[0m");
    return NULL;
}

// TODO: translate Varlist
char *translateVarList(Morpheme *varList, HashSet *symTable)
{
    if (varList == NULL || varList->type != _VarList)
    {
        printf("\033[31mBad VarList node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = varList->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty VarList node.\n\033[0m");
        return NULL;
    }
    // VarList -> ParamDec Comma VarList
    else if (c->type == _ParamDec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _VarList && c->siblings->siblings->siblings == NULL)
    {
        char *code1 = translateParamDec(c, symTable);
        char *code2 = translateVarList(c->siblings->siblings, symTable);
        char *code = concat(2, code1, code2);
        return code;
    }
    // VarList -> ParamDec
    else if (c->type == _ParamDec && c->siblings == NULL)
    {
        char *code = translateParamDec(c, symTable);
        return code;
    }
    return NULL;
}

char *translateParamDec(Morpheme *paramDec, HashSet *symTable)
{
    if (paramDec == NULL || paramDec->type != _ParamDec)
    {
        printf("\033[31mBad ParamDec node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = paramDec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty ParamDec node.\n\033[0m");
        return NULL;
    }
    else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _VarDec)
    {
        char *code = translateVarDecFromParamDec(c->siblings, symTable);
        return code;
    }

    printf("\033[31mBad ParamDec node.\n\033[0m");
    return NULL;
}
char *translateCompSt(Morpheme *compSt, HashSet *symTable)
{
    //printf("Translating Compst\n");
    if (compSt == NULL || compSt->type != _CompSt)
    {
        printf("\033[31mBad Compst node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = compSt->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty CompSt node.\n\033[0m");
        return NULL;
    }
    // CompSt -> { DefList StmtList }
    if (c->type == _LC && c->siblings != NULL && c->siblings->type == _DefList && c->siblings->siblings != NULL && c->siblings->siblings->type == _StmtList && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RC)
    {
        char *code1, *code2;
        code1 = translateDefList(c->siblings, symTable);
        code2 = translateStmtList(c->siblings->siblings, symTable);
        char *code = concat(2, code1, code2);
        return code;
    }
}
char *translateStmtList(Morpheme *stmtList, HashSet *symTable)
{
    //printf("Translating stmtlist\n");
    if (stmtList == NULL || stmtList->type != _StmtList)
    {
        printf("\033[31mBad stmtList node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = stmtList->child;
    //stmtlist -> empty
    if (c->type == _BLANK)
    {
        return NULL;
    }
    //stmtlist -> stmt stmtlist
    else if (c->type == _Stmt && c->siblings != NULL && c->siblings->type == _StmtList)
    {
        char *code;
        char *code1 = translateStmt(c, symTable);
        char *code2 = translateStmtList(c->siblings, symTable);
        code = concat(2, code1, code2);
        return code;
    }
}

char *translateDefList(Morpheme *defList, HashSet *symTable)
{
    //printf("Translating Def list\n");
    if (defList == NULL || defList->type != _DefList)
    {
        printf("\033[31mBad defList node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = defList->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty defList node.\n\033[0m");
        return NULL;
    }
    // DefList -> empty
    else if (c->type == _BLANK)
    {
        return NULL;
    }
    // DefList -> Def DefList
    else if (c->type == _Def && c->siblings != NULL && c->siblings->type == _DefList && c->siblings->siblings == NULL)
    {
        char *code, *code1, *code2;
        code1 = translateDef(c, symTable);
        code2 = translateDefList(c->siblings, symTable);
        code = concat(2, code1, code2);
        return code;
    }
    printf("\033[31mBad DefList node.\n\033[0m");
    return NULL;
}
char *translateDef(Morpheme *def, HashSet *symTable)
{
    if (def == NULL || def->type != _Def)
    {
        printf("\033[31mBad def node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = def->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty stmtList node.\n\033[0m");
        return NULL;
    }
    // Def -> Specifier DecList Semi
    else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _DecList && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI && c->siblings->siblings->siblings == NULL)
    {
        //由于我们已经进行了语义分析，所以Specifier中的信息已经获取了，直接处理DecList
        char *code = translateDecList(c->siblings, symTable);
        //printf("def code = %s", code);
        return code;
    }
}
char *translateDecList(Morpheme *decList, HashSet *symTable)
{
    if (decList == NULL || decList->type != _DecList)
    {
        printf("\033[31mBad decList node.\n\033[0m");
        return NULL;
    }
    Morpheme *c = decList->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty DecList node.\n\033[0m");
        return NULL;
    }
    // DecList -> Dec
    else if (c->type == _Dec && c->siblings == NULL)
    {
        char *code = translateDec(c, symTable);
        //printf("deccode = %s", code);
        return code;
    }
    // DecList -> Dec Comma DecList
    else if (c->type == _Dec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _DecList && c->siblings->siblings->siblings == NULL)
    {
        char *code1, *code2;
        code1 = translateDec(c, symTable);
        code2 = translateDecList(c->siblings->siblings, symTable);
        char *code = concat(2, code1, code2);
        return code;
    }
}
char *translateDec(Morpheme *dec, HashSet *symTable)
{
    if (dec == NULL || dec->type != _Dec)
    {
        printf("\033[31mBad dec node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = dec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty dec node.\n\033[0m");
        return NULL;
    }
    // Dec -> VarDec
    else if (c->type == _VarDec && c->siblings == NULL)
    {
        return translateVarDec(c, symTable);
    }
    // Dec -> VarDec ASSIGNOP EXP
    else if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        char *symbolName = "";
        char *code1 = translateVarDecWithAssignop(c, symTable, &symbolName);
        Symbol *s = get(symTable, symbolName);
        char * temp = getTemp();
        char *code2 = translateExp(c->siblings->siblings, symTable, temp);
        char code3[256];
        sprintf(code3, "%s := %s\n", s->variable, temp);
        char *code = concat(3, code1, code2, code3);
        return code;
    }
}
//没有初始化赋值的变量声明
char *translateVarDec(Morpheme *varDec, HashSet *symTable)
{
    if (varDec == NULL || varDec->type != _VarDec)
    {
        printf("\033[31mBad varDec node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = varDec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty vardec node.\n\033[0m");
        return NULL;
    }
    // VarDec -> ID
    else if (c->type == _ID && c->siblings == NULL)
    {
        char *name = c->idName;
        Symbol *s = get(symTable, name);
        switch (s->symbol_type)
        {
        //只有结构体和数组我们分配空间，int和float的话不分配空间
        case STRUCT_VAL_SYMBOL:
        {
            char *variable = s->variable;
            int size = calcSize(s->name);
            //printf("Size = %d\n", size);
            char *code;
            char str[256];
            char str2[256];
            sprintf(str, "%s %s %d\n", "DEC", variable, size);
            char *newVariable = getVariable();
            sprintf(str2, "%s := &%s\n", newVariable, variable);
            overwriteVariable(symTable, s->name, newVariable);
            code = concat(2, str, str2);
            return code;
        }
        case ARRAY_SYMBOL:
        {
            char *variable = s->variable;
            int size = calcSize(s->name);
            //printf("Size = %d\n", size);
            char *code;
            char str[256];
            char str2[256];
            sprintf(str, "%s %s %d\n", "DEC", variable, size);
            char *newVariable = getVariable();
            sprintf(str2, "%s := &%s\n", newVariable, variable);
            overwriteVariable(symTable, s->name, newVariable);
            code = concat(2, str, str2);
            //printf("code = %s\n", code);
            return code;
        }
        default:
        {
            return NULL;
        }
        }
    }
    // VarDec -> VarDec LB INT RB
    else if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _LB && c->siblings->siblings != NULL && c->siblings->siblings->type == _INT && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RB)
    {
        return translateVarDec(c, symTable);
    }
}

//有初始化赋值的变量声明， 通过symbolName返回变量的名称
char *translateVarDecWithAssignop(Morpheme *varDec, HashSet *symTable, char **symbolName)
{
    if (varDec == NULL || varDec->type != _VarDec)
    {
        printf("\033[31mBad varDec node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = varDec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty vardec node.\n\033[0m");
        return NULL;
    }

    //由于数组和结构体不会被直接赋值，所以此时的vardec一定是一个ID
    if (c->type == _ID && c->siblings == NULL)
    {
        *symbolName = c->idName;
        Symbol *s = get(symTable, c->idName);
        if (s->symbol_type != INT_SYMBOL && s->symbol_type != FLOAT_SYMBOL)
        {
            printf("\033[31mUnsupport symbol type\n\033[0m");
            return NULL;
        }
        return NULL;
    }
    printf("\033[31mUnsupport vardec\n\033[0m");
    return NULL;
}

//翻译形参中的vardec
char *translateVarDecFromParamDec(Morpheme *varDec, HashSet *symTable)
{
    if (varDec == NULL || varDec->type != _VarDec)
    {
        printf("\033[31mBad VarDec node.\n\033[0m");
        return NULL;
    }

    Morpheme *c = varDec->child;
    if (c == NULL)
    {
        printf("\033[31mEmpty vardec node.\n\033[0m");
        return NULL;
    }

    //由于数组和结构体不会被直接赋值，所以此时的vardec一定是一个ID
    if (c->type == _ID && c->siblings == NULL)
    {
        char *name = c->idName;
        Symbol *s = get(symTable, name);
        char *code = concat(3, "PARAM ", s->variable, " \n");
        return code;
    }
    printf("\033[31mCan not translate: Unsupport parameter's type.\n\033[0m");
    exit(-1);
    return NULL;
}
