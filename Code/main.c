#include <stdio.h>
#include "grammarTree.h"
#include "semantic.h"
#include "symbol.h"
#include "hashset.h"
#include "log.h"
extern void yyrestart(FILE *);
extern void yyparse(void);
extern Morpheme *root;
extern int syntax_correct;
extern int lexical_correct;
extern HashSet *symbolTable;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("pass filename to scanner\n");
        return -1;
    }
    else
    {
        FILE *f = fopen(argv[1], "r");
        if (!f)
        {
            printf("fopen failed:%s\n", argv[1]);
            return -1;
        }
        yyrestart(f);
        yyparse();
        //printTotalGrammarTree(root, 0);
        if (syntax_correct && lexical_correct)
        {
            symbolTable = initializeHashSet(HASH_SIZE);
            SemanticAnalysisLog = initLog();
            SemanticError = initLog();
            //printTotalGrammarTree(root, 0);
            handleProgram(root);
            //outputLog(SemanticAnalysisLog);
            //outputHashSet(symbolTable);
            outputLog(SemanticError);
        }
        destructMorpheme(root);
        return 0;
    }
    /*
    char s1[] = "asadb";
    char s2[] = "abc";
    char s3[] = "1234";
    printf("%u\n", pjwHash(s1));
    printf("%u\n", pjwHash(s2));
    printf("%u\n", pjwHash(s3));
    HashSet* set = initializeHashSet(HASH_SIZE);
    printf("%d\n", isContain(set, s1));
    Symbol* s = createSymbol(s1);
    printf("%d\n", insert(set, s));
    printf("%d\n", isContain(set, s1));
    Symbol* p = get(set, "asadb");
    printf("%s\n", p->name);
    printf("%d\n", insert(set, s));*/

    /*symbolTable = initializeHashSet(HASH_SIZE);
    Symbol* s1 = createSymbol();
    setSymbolName(s1, "main");
    setSymbolType(s1, FUNC_SYMBOL);
    setFuncReturnValue(s1, _INT_TYPE_, "");
	Symbol* s2 = createSymbol();
	setSymbolName(s2, "x1");
	setSymbolType(s2, INT_SYMBOL);
	insert(symbolTable, s2);
	Symbol* s3 = createSymbol();
	setSymbolName(s3, "x2");
	setSymbolType(s3, INT_SYMBOL);
	insert(symbolTable, s3);
    addFuncArgument(s1, "x1");
    addFuncArgument(s1, "x2");
    insert(symbolTable, s1);
    Symbol* s4 = createSymbol();
    setSymbolName(s4, "p");
    setSymbolType(s4, INT_SYMBOL);
    insert(symbolTable, s4);
    Symbol* s5 = createSymbol();
    setSymbolName(s5, "i");
    setSymbolType(s5, INT_SYMBOL);
    insert(symbolTable, s5);
    Symbol* s6 = createSymbol();
    setSymbolName(s6, "i");
    setSymbolType(s6, INT_SYMBOL);
    insert(symbolTable, s6);

    outputHashSet(symbolTable);*/
}
