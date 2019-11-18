#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int global_temp_num = 0;
int global_variable_num = 0;
int global_label_num = 0;

char* concat(int n, ...) {
    va_list argp;
    va_start(argp, n);
    int length = 0;
    for (int i = 0; i < n; i++) {
        char* s = va_arg(argp, char*);
        if(s != NULL) {
            length += strlen(s);
        }
    }
    va_end(argp);
    char* str = (char*) malloc(length + 1);
    memset(str, length, 0);
    va_start(argp, n);
    for (int i = 0; i < n; i++) {
        char* s = va_arg(argp, char*);
        if(s != NULL) {
            strcat(str, s);
        }
    }
    return str;
}

char* getTemp() {
    char str[256];
    sprintf(str, "t%d", global_temp_num++);
    char* temp = (char*) malloc(256);
    strcpy(temp, str);
    return temp;
}

char* getVariable() {
    char str[256];
    sprintf(str, "v%d", global_variable_num++);
    char* temp = (char*) malloc(256);
    strcpy(temp, str);
    return temp;
}

char* getLabel() {
    char str[256];
    sprintf(str, "label%d", global_label_num++);
    char* temp = (char*) malloc(256);
    strcpy(temp, str);
    return temp;
}

