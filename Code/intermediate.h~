#ifndef __INTERMEDIATE_H_
#define __INTERMEDIATE_H_
#include "grammarTree.h"
#include "hashset.h"
char* translateExp(Morpheme* exp, HashSet* symTable, char* place);
char* translateStmt(Morpheme* stmt, HashSet* symTable);
char* translateCond(Morpheme* exp, char* label_true, char* label_false, HashSet* symTable);
char* translateFunc(Morpheme* exp, HashSet* symTable, char* place);
char* translateArgs(Morpheme* args, HashSet* symtable);
#endif
