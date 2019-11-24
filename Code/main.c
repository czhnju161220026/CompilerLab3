#include <stdio.h>
#include "grammarTree.h"
#include "semantic.h"
#include "symbol.h"
#include "hashset.h"
#include "log.h"
#include "utils.h"
#include "intermediate.h"
extern void yyrestart(FILE *);
extern void yyparse(void);
extern Morpheme *root;
extern int syntax_correct;
extern int lexical_correct;
extern HashSet *symbolTable;

int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        printf("pass filename to scanner and output path\n");
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
            addReadAndWrite(symbolTable);
            SemanticAnalysisLog = initLog();
            SemanticError = initLog();
            //printTotalGrammarTree(root, 0);
            handleProgram(root);
            //outputLog(SemanticAnalysisLog);
            //outputHashSet(symbolTable);
            outputLog(SemanticError);
            char* code = translateProgram(root, symbolTable);
            //printf("%s", code);
            FILE* ir_code= fopen(argv[2], "w");
            fprintf(ir_code, "%s", code);
            fclose(ir_code);
        }
        destructMorpheme(root);
        fclose(f);
        return 0;
    }
}
