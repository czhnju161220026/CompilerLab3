#ifndef __LOG_H_
#define __LOG_H_
#include <stdbool.h>
typedef struct Info {
    char* content;
    struct Info* next;
} Info;

typedef struct Log {
    Info* head;
    Info* tail;
} Log;

extern Log* SemanticError;
extern Log* SemanticAnalysisLog;
Log* initLog();
bool addLogInfo(Log* log, char* content);
bool outputLog(Log* log);
bool reportError(Log* log, int type, int line, char* message);


#endif
