#include "intermediate.h"
#include "grammarTree.h"
#include "hashset.h"
#include "semantic.h"
#include <stdio.h>
#include <string.h>
char* translateExp(Morpheme* exp, HashSet* symTable, char* place) {
    if (exp == NULL) {
        printf("\033[31mThis exp is NULL\n\033[0m");
        return NULL;
    }
    if (exp->type != _EXP) {
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
    } else if (c->type == _EXP && c->siblings != NULL && c->siblings->type == _ASSIGNOP && s->siblings->siblings != NULL && c->siblings->siblings->type == _EXP) {
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
    } else if (c->type == _EXP && c->siblings != NULL && (c->siblings->type == _PLUS || c->siblings->type == _MINUS || c->siblings->type == _STAR || c->siblings->type == _DIV) && s->siblings->siblings != NULL && c->siblings->siblings->type == _EXP) {
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
        }else if (c->siblings->type == _DIVS) {
            sprintf(str, "%s := %s / %s\n", place, t1, t2);
        }
        char* code3 = (char*) malloc(strlen(str) + 1);
        strcpy(code3, str);
        return concat(3, code1, code2, code3);
    } else if (c->type == _MINUS && c->siblings != NULL && c->siblings->type == _EXP) {
        char* t1 = getTemp();
        char* code1 = translateExp(c->siblings, symTable, t1);
        char str[256];
        sprintf(str, "%s := #0 - %s\n", place, t1);
        char* code2 = (char*) malloc(strlen(str) + 1);
        strcpy(code2, str);
        return concat(2, code1, code2)
    } else if ((c->type == _EXP && c->siblings != NULL && (c->siblings->type == _RELOP || c->siblings->type == _AND || c->siblings->type == _OR) && s->siblings->siblings != NULL && c->siblings->siblings->type == _EXP) || (c->type == _NOT && c->siblings != NULL && c->siblings->type == _EXP)) {
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
    }
    return NULL;
}

char* translateStmt(Morpheme* stmt, HashSet* symTable) {
    return NULL;
}

char* translateCond(Morpheme* exp, char* label_true, char* label_false, HashSet* symTable) {
    return NULL;
}

char* translateFunc(Morpheme* exp, HashSet* symTable, char* place) {
    return NULL;
}

char* translateArgs(Morpheme* args, HashSet* symtable) {
    return NULL;
}
