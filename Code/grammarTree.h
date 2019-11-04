#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#ifndef __TREE_H_
#define __TREE_H_
#define MAX_CHILDREN_NUMBER 8

extern const char *const TYPES_NAME_TABLE[];

typedef enum Types
{
	/*0 Blank*/
	_BLANK,
	/*1 Tokens*/
	_SEMI,
	_COMMA,
	_ASSIGNOP,
	_RELOP,
	_PLUS,
	_MINUS,
	_STAR,
	_DIV,
	_AND,
	_OR,
	_DOT,
	_NOT,
	_TYPE,
	_LP,
	_RP,
	_LB,
	_RB,
	_LC,
	_RC,
	_STRUCT,
	_RETURN,
	_IF,
	_ELSE,
	_WHILE,
	_ID,
	_INT,
	_FLOAT,
	/*2 High-level Definitions*/
	_Program,
	_ExtDefList,
	_ExtDef,
	_ExtDecList,
	/*3 Specifiers*/
	_Specifier,
	_StructSpecifier,
	_OptTag,
	_Tag,
	/*4 Declarators*/
	_VarDec,
	_FunDec,
	_VarList,
	_ParamDec,
	/*5 Statements*/
	_CompSt,
	_StmtList,
	_Stmt,
	/*6 Local Definitions*/
	_DefList,
	_Def,
	_DecList,
	_Dec,
	/*7 Expressions*/
	_Exp,
	_Args
} Types;

typedef struct Morpheme
{
	Types type;
	union {
		char *idName;
		int intValue;
		float floatValue;
	};
	int lineNumber;
	struct Morpheme *father;
	//struct Morpheme* children[MAX_CHILDREN_NUMBER];
	struct Morpheme *child;
	struct Morpheme *siblings;
} Morpheme;

Morpheme *createMorpheme(Types type);
void nodeGrowth(Morpheme *father, int n, ...);
void printGrammarTree(Morpheme *root, int depth);
void destructMorpheme(Morpheme *morpheme);
const char *typeToString(Types type);
#endif
