#ifndef __SEMACTIC_H_
#define __SEMACTIC_H_
#include "hashset.h"
#include "grammarTree.h"
#include "symbol.h"
void printTotalGrammarTree(Morpheme *root, int depth);

bool handleProgram(Morpheme *root);
bool handleExtDefList(Morpheme *root);
bool handleExtDef(Morpheme *root);
bool handleSpecifier(Morpheme *root, ValueTypes *type, char **name);
bool handleTYPE(Morpheme *root, ValueTypes *type);
bool handleStructSpecifier(Morpheme *root, ValueTypes *type, char **name);
bool handleTag(Morpheme *root, char **name);
bool handleOptTag(Morpheme *root, char **name);
bool handleExtDecList(Morpheme *root, ValueTypes *type, char **name);
bool handleVarDec(Morpheme *root, Symbol *s);
bool handleDefList(Morpheme *root, Symbol *s);
bool handleDef(Morpheme *root, Symbol *s);
bool handleDecList(Morpheme *root, Symbol *s, ValueTypes *type, char **name);
bool handleDec(Morpheme *root, Symbol *s, Symbol *field);
bool handleFunDec(Morpheme *root, Symbol *s);
bool handleVarList(Morpheme *root, Symbol *s);
bool handleParamDec(Morpheme *root, Symbol *s);
bool handleCompSt(Morpheme *root);
bool handleStmtList(Morpheme *root);
bool handleStmt(Morpheme *root);
bool handleExp(Morpheme* root, ExpType* expType);
bool handleArgs(Morpheme* root, ParaType* parameters);
#endif
