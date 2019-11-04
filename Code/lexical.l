/* flex源文件由3部分构成，用%%分开*/
%{
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "grammarTree.h"
#define YYSTYPE Morpheme*
#include "syntax.tab.h"  
#ifndef __DEBUG_ 
#define __DEBUG_ 0
#endif
/*void discard_comment();*/
int is_split_char(char c);
int lexical_correct = 1; 
%}
/*定义部分*/
/*Flex采用最长规则进行匹配*/
/*注释识别和指数浮点数的识别不属于我们的选作范围，所以将其注释*/
%option yylineno
/*Exp_float (([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE][+-]?[0-9]+)*/ 
/*把ID的标准放宽松，识别出后再判断是否合法*/
Float ([0-9]+\.[0-9]+)
Dec_number (([1-9][0-9]*)|0) 
Oct_number 0[0-7]+
Illegal_Oct 0[0-7]*[89]+[0-9]*
Hex_number 0[Xx][0-9a-fA-F]+
Illegal_Hex 0[Xx][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
ID [_a-zA-Z0-9]+
Semi ;
Comma ,
Assign_op =
Rel_op (>)|(<)|(>=)|(<=)|(==)|(!=)
Plus \+
Minus -
Star \*
Div \/
And &&
Or \|\|
Dot \.
Not !
Lp \(
Rp \)
Lb \[
Rb \]
Lc \{
Rc \}
Blank [ \r\t]+
/*OneLineComment \/\/[^\n]* */
/*MultiLineComment \/\*  */
/*规则部分: 定义匹配时采取的动作*/
/*越靠前优先级越高，匹配时按照贪心的原则进行，所以关键字如int float 应该放在前面*/
/*{OneLineComment} {if(__DEBUG_) printf("Line %d: match a OneLineComment\n", yylineno, yytext);}*/
/*{MultiLineComment} {discard_comment(); if(__DEBUG_) printf("Line %d: match a MultiLineComment\n", yylineno, yytext);} */
/*{Exp_float} {if (__DEBUG_) printf("Line %d: match a float: %s\n", yylineno, yytext);*/
    /*yylval=createMorpheme(_FLOAT); yylval->lineNumber = yylineno; yylval->floatValue = strtof(yytext,NULL);return FLOAT;}*/
%%
{Float} {if (__DEBUG_) printf("Line %d: match a float: %s\n", yylineno, yytext);
    yylval=createMorpheme(_FLOAT); yylval->lineNumber = yylineno; yylval->floatValue = strtof(yytext,NULL);return FLOAT;}
{Dec_number} {if (__DEBUG_) printf("Line %d: match a dec number: %s\n", yylineno, yytext); 
    yylval=createMorpheme(_INT); yylval->lineNumber = yylineno; yylval->intValue = strtol(yytext, NULL, 10); return INT;}
{Illegal_Oct} {if (__DEBUG_)  printf("Line %d: match an illegal oct number: %s\n", yylineno, yytext);
    printf("Error type A at Line %d: Illegal oct number \'%s\' \n", yylineno, yytext);
    lexical_correct = 0;
    yylval=createMorpheme(_INT); yylval->lineNumber = yylineno;yylval->intValue = 0;return INT;
}
{Oct_number} {if (__DEBUG_)  printf("Line %d: match an oct number: %s\n", yylineno, yytext);
    yylval=createMorpheme(_INT); yylval->lineNumber = yylineno;yylval->intValue = strtol(yytext, NULL, 8);return INT;}
{Illegal_Hex} {if (__DEBUG_)  printf("Line %d: match an illegal hex number: %s\n", yylineno, yytext);
    printf("Error type A at Line %d: Illegal hex number \'%s\' \n", yylineno, yytext);
    lexical_correct = 0;
    yylval=createMorpheme(_INT); yylval->lineNumber = yylineno;yylval->intValue = 0;return INT;
}
{Hex_number} {if (__DEBUG_)  printf("Line %d: match a hex number: %s\n", yylineno, yytext);
    yylval=createMorpheme(_INT); yylval->lineNumber = yylineno;yylval->intValue = strtol(yytext, NULL, 16);return INT;}
{ID} {
    if(yytext[0] >='0' && yytext[0] <= '9') {
        printf("Error type A at Line %d: Illegal id \'%s\' \n", yylineno, yytext);
        lexical_correct = 0;
        yylval=createMorpheme(_ID); yylval->lineNumber = yylineno;
        yylval->idName=strcpy((char*)malloc(sizeof(char)*(strlen(yytext)+1)), yytext);
        return ID; //不影响语法分析，仍然返回一个ID token
    }
    else if(strcmp("int", yytext) == 0 || strcmp("float", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a type: %s\n", yylineno, yytext);
        yylval=createMorpheme(_TYPE); yylval->lineNumber = yylineno;
        yylval->idName=strcpy((char*)malloc(sizeof(char)*(strlen(yytext)+1)), yytext);
        return TYPE;
    }
    else if(strcmp("struct", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a struct: %s\n", yylineno, yytext);
        yylval=createMorpheme(_STRUCT); yylval->lineNumber = yylineno;
        return STRUCT;
    }
    else if(strcmp("return", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a return: %s\n", yylineno, yytext);
        yylval=createMorpheme(_RETURN); yylval->lineNumber = yylineno;
        return RETURN;
    }
    else if(strcmp("if", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a if: %s\n", yylineno, yytext);
        yylval=createMorpheme(_IF); yylval->lineNumber = yylineno;
        return IF;
    }
    else if(strcmp("else", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a else: %s\n", yylineno, yytext);
        yylval=createMorpheme(_ELSE); yylval->lineNumber = yylineno;
        return ELSE;
    }
    else if(strcmp("while", yytext) == 0) {
        if (__DEBUG_) printf("Line %d: match a while: %s\n", yylineno, yytext);
        yylval=createMorpheme(_WHILE); yylval->lineNumber = yylineno;
        return WHILE;
    }
    else {
        if (__DEBUG_) printf("Line %d: match a id: %s\n", yylineno, yytext);
        yylval=createMorpheme(_ID); yylval->lineNumber = yylineno;
        yylval->idName=strcpy((char*)malloc(sizeof(char)*(strlen(yytext)+1)), yytext);
        return ID;
    }
}
{Semi} {if (__DEBUG_) printf("Line %d: match a semi: %s\n", yylineno, yytext);
    yylval=createMorpheme(_SEMI); yylval->lineNumber = yylineno;return SEMI;}
{Comma} {if (__DEBUG_) printf("Line %d: match a comma: %s\n", yylineno, yytext);
    yylval=createMorpheme(_COMMA); yylval->lineNumber = yylineno;return COMMA;}
{Assign_op} {if (__DEBUG_) printf("Line %d: match a assignop: %s\n", yylineno, yytext);
    yylval=createMorpheme(_ASSIGNOP); yylval->lineNumber = yylineno;return ASSIGNOP;}
{Rel_op} {if (__DEBUG_) printf("Line %d: match a relop: %s\n", yylineno, yytext);
    yylval=createMorpheme(_RELOP); yylval->lineNumber = yylineno;
    yylval->idName=strcpy((char*)malloc(sizeof(char)*(strlen(yytext)+1)), yytext);return RELOP;}
{Plus} {if (__DEBUG_) printf("Line %d: match a plus: %s\n", yylineno, yytext);
    yylval=createMorpheme(_PLUS); yylval->lineNumber = yylineno;return PLUS;}
{Minus} {if (__DEBUG_) printf("Line %d: match a minus: %s\n", yylineno, yytext);
    yylval=createMorpheme(_MINUS); yylval->lineNumber = yylineno;return MINUS;}
{Star} {if (__DEBUG_) printf("Line %d: match a start: %s\n", yylineno, yytext);
    yylval=createMorpheme(_STAR); yylval->lineNumber = yylineno;return STAR;}
{Div} {if (__DEBUG_) printf("Line %d: match a div: %s\n", yylineno, yytext);
    yylval=createMorpheme(_DIV); yylval->lineNumber = yylineno;return DIV;}
{And} {if (__DEBUG_) printf("Line %d: match a and: %s\n", yylineno, yytext);
    yylval=createMorpheme(_AND); yylval->lineNumber = yylineno;return AND;} 
{Or} {if (__DEBUG_) printf("Line %d: match a or: %s\n", yylineno, yytext);
    yylval=createMorpheme(_OR); yylval->lineNumber = yylineno;return OR;} 
{Dot} {if (__DEBUG_) printf("Line %d: match a dot: %s\n", yylineno, yytext);
    yylval=createMorpheme(_DOT); yylval->lineNumber = yylineno;return DOT;}
{Not} {if (__DEBUG_) printf("Line %d: match a not: %s\n", yylineno, yytext);
    yylval=createMorpheme(_NOT); yylval->lineNumber = yylineno;return NOT;}
{Lp} {if (__DEBUG_) printf("Line %d: match a lp: %s\n", yylineno, yytext);
    yylval=createMorpheme(_LP); yylval->lineNumber = yylineno;return LP;}
{Rp} {if (__DEBUG_) printf("Line %d: match a rp: %s\n", yylineno, yytext);
    yylval=createMorpheme(_RP); yylval->lineNumber = yylineno;return RP;}
{Lb} {if (__DEBUG_) printf("Line %d: match a lb: %s\n", yylineno, yytext);
    yylval=createMorpheme(_LB); yylval->lineNumber = yylineno;return LB;}
{Rb} {if (__DEBUG_) printf("Line %d: match a rb: %s\n", yylineno, yytext);
    yylval=createMorpheme(_RB); yylval->lineNumber = yylineno;return RB;}
{Lc} {if (__DEBUG_) printf("Line %d: match a lc: %s\n", yylineno, yytext);
    yylval=createMorpheme(_LC); yylval->lineNumber = yylineno;return LC;}
{Rc} {if (__DEBUG_) printf("Line %d: match a rc: %s\n", yylineno, yytext);
    yylval=createMorpheme(_RC); yylval->lineNumber = yylineno;return RC;}
{Blank} {} //忽略空格和tab
. {printf("Error type A at Line %d: mysterious characters \'%s\' \n", yylineno, yytext);} //对于未定义字符报错处理
\n {} //添加该规则以启用yylineno

%%
/*用户自定义代码*/
//丢弃多行注释
/*void discard_comment() {
    char c1, c2;
    while(1) {
        c1 = input();
        while(c1 != '*') {c1 = input();}
        c2 = input();
        if(c2 == '/') {
            break;
        }
    }
}*/
/*
int is_split_char(char c) {
     if(c!=' ' && c != '\t' && c != '\n' && c != ';' && c != ',') {
         return 0;
     }
     return 1;
}

int is_operator(char c) {
    if(c != '+' && c != '-' && c != '*' && c != '/' && c != '>' && c != '<' && c != '=') {
        return 0;
    }
    return 1;
}
*/

/* SLT ADD A COMMENT */
