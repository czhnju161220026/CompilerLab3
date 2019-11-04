#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

Log* SemanticError = NULL;
Log* SemanticAnalysisLog = NULL;
Log* initLog() {
    Log* log = (Log*) malloc(sizeof(Log));
    if (log != NULL) {
	log->head = NULL;
        log->tail = NULL;
        return log;
    } else {
        return NULL;
    }
    
}
bool addLogInfo(Log* log, char* content) {
    //printf("%s\n", content);
    if (log == NULL) {
        printf("The Log is NULL.\n");
	return false;
    } else {
        if (log->head == NULL && log->tail == NULL) {
            Info* i = (Info*) malloc(sizeof(Info));
            i->content = (char*) malloc(strlen(content) + 1);
            strcpy(i->content, content);
            i->next = NULL;
            log->head = i;
            log->tail = i;
        } else if (log->head != NULL && log->tail != NULL) {
            Info* i = (Info*) malloc(sizeof(Info));
            i->content = (char*) malloc(strlen(content) + 1);
            strcpy(i->content, content);
            i->next = NULL;
            log->tail->next = i;
            log->tail = i;
        } else {
            printf("This Log is broken.\n");
            return false;
        }
    }
}
bool outputLog(Log* log) {
    if (log == NULL) {
        printf("The Log is NULL.\n");
	return false;
    }
    Info* p = log->head;
    while (p != NULL) {
        printf("%s", p->content);
        p = p->next;
    }
    return true;
}

bool reportError(Log* log, int type, int line, char* message) {
    char str[256];
    sprintf(str, "Error Type %d at Line %d: %s.\n", type, line, message);
    return addLogInfo(log, str);
}
