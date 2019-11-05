#ifndef __INTERMEDIATE_H_
#define __INTERMEDIATE_H_
#include "grammarTree.h"
#include "hashset.h"
char* translateExp(Morpheme* exp, HashSet* symTable, char* place);
char* translateStmt(Morpheme* stmt, HashSet* symTable);
char* translateCond(Morpheme* exp, char* label_true, char* label_false, HashSet* symTable);
char* translateFunc(Morpheme* exp, HashSet* symTable, char* place);
char* translateArgs(Morpheme* args, HashSet* symTable, Argument* arglist);
char* translateProgram(Morpheme* prog, HashSet* symTable);
char* translateExtDefList(Morpheme* extDefList, HashSet* symTable);
char* translateExtDef(Morpheme* extDef, HashSet* symTable);
char* translateFunDec(Morpheme* m);
char* translateCompSt(Morpheme* m);
char* translateStmtList(Morpheme* m);
char* translateDefList(Morpheme* m);
char* translateDef(Morpheme* m);
char* translateDecList(Morpheme* m);
char* translateDec(Morpheme* m);
#endif
