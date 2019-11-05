#include "intermediate.h"
#include "grammarTree.h"
#include "hashset.h"
#include "semantic.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
char* translateExp(Morpheme* exp, HashSet* symTable, char* place) {
    if (exp == NULL) {
        printf("\033[31mThis exp is NULL\n\033[0m");
        return NULL;
    }
    if (exp->type != _Exp) {
        printf("\033[31mThis exp is not _EXP\n\033[0m");
        return NULL;
    }
    Morpheme* c = exp->child;
    if (c == NULL) {
        printf("\033[31mThis child is NULL\n\033[0m");
        return NULL;
    }
    if (c->type == _INT) {
        int value = c->intValue;
        char str[256];
        sprintf(str, "%s := #%d\n", place, value);
        char* code = (char*) malloc(strlen(str) + 1);
        strcpy(code, str);
        return code;
    } else if (c->type == _ID) {
        Symbol* s = get(symTable, c->idName);
        char* variable = s->variable;
        char str[256];
        sprintf(str, "%s := %s\n", place, variable);
        char* code = (char*) malloc(strlen(str) + 1);
        strcpy(code, str);
        return code;
    } else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp) {
        if (c->child->type == _ID) {
            Symbol* s = get(symTable, c->child->idName);
            char* variable = s->variable;
            char* t1 = getTemp();
            char* code1 = translateExp(c->siblings->siblings, symTable, t1);
            char str[256];
            sprintf(str, "%s := %s\n%s := %s", variable, t1, place, variable);
            char* code2 = (char*) malloc(strlen(str) + 1);
            strcpy(code2, str);
            return concat(2, code1, code2);
        }
    } else if (c->type == _Exp && c->siblings != NULL && (c->siblings->type == _PLUS || c->siblings->type == _MINUS || c->siblings->type == _STAR || c->siblings->type == _DIV) && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp) {
        char* t1 = getTemp();
        char* t2 = getTemp();
        char* code1 = translateExp(c, symTable, t1);
        char* code2 = translateExp(c->siblings->siblings, symTable, t2);
        char str[256];
        if (c->siblings->type == _PLUS) {
            sprintf(str, "%s := %s + %s\n", place, t1, t2);
        } else if (c->siblings->type == _MINUS) {
            sprintf(str, "%s := %s - %s\n", place, t1, t2);
        }else if (c->siblings->type == _STAR) {
            sprintf(str, "%s := %s * %s\n", place, t1, t2);
        }else if (c->siblings->type == _DIV) {
            sprintf(str, "%s := %s / %s\n", place, t1, t2);
        }
        char* code3 = (char*) malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code2, code3);
    } else if (c->type == _MINUS && c->siblings != NULL && c->siblings->type == _Exp) {
        char* t1 = getTemp();
        char* code1 = translateExp(c->siblings, symTable, t1);
        char str[256];
        sprintf(str, "%s := #0 - %s\n", place, t1);
        char* code2 = (char*) malloc(strlen(str) + 1);
        strcpy(code2, str);
        return concat(2, code1, code2);
    } else if ((c->type == _Exp && c->siblings != NULL && (c->siblings->type == _RELOP || c->siblings->type == _AND || c->siblings->type == _OR) && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp) || (c->type == _NOT && c->siblings != NULL && c->siblings->type == _Exp)) {
        char* label1 = getLabel();
        char* label2 = getLabel();
        char str[256];
        sprintf(str, "%s := #0\n", place);
        char* code0 = (char*) malloc(strlen(str) + 1);
        strcpy(code0, str);

        char* code1 = translateCond(c, label1, label2, symTable);
        sprintf(str, "LABEL %s :\n%s := 1\n", label1, place);
        char* code2 = (char*) malloc(strlen(str) + 1);
        strcpy(code2, str);

        sprintf(str, "LABEL %s :\n", label2);
        char* code3 = (char*) malloc(strlen(str) + 1);
        strcpy(code3, str);

        return concat(4, code0, code1, code2, code3);
    } else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP) {
        if (strcmp(c->idName, "read") == 0) {
            char str[256];
            sprintf(str, "READ %s\n", place);
            char* code = (char*) malloc(strlen(str) + 1);
            strcpy(code, str);
            return code;
        } else {
            char str[256];
            sprintf(str, "%s := CALL %s\n", place, c->idName);
            char* code = (char*) malloc(strlen(str) + 1);
            strcpy(code, str);
            return code;
        }
    } else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Args && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP) {
        //TODO
    }

    return NULL;
}

char* translateStmt(Morpheme* stmt, HashSet* symTable) {
    return NULL;
}

char* translateCond(Morpheme* exp, char* label_true, char* label_false, HashSet* symTable) {
    return NULL;
}


char* translateArgs(Morpheme* args, HashSet* symtable) {
    return NULL;
}

char* translateProgram(Morpheme* m);
char* translateExtDefList(Morpheme* m);
char* translateExtDef(Morpheme* m);
char* translateFunDec(Morpheme* m);
char* translateCompSt(Morpheme* m);
char* translateStmtList(Morpheme* m);
char* translateDefList(Morpheme* m);
char* translateDef(Morpheme* m);
char* translateDecList(Morpheme* m);
char* translateDec(Morpheme* m);
