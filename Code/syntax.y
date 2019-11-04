%{
#include <stdio.h>
#include "lex.yy.c"
#include "grammarTree.h"

Morpheme* root = NULL;

#ifndef YYSTYPE
#define YYSTYPE Morpheme*
#endif
#define ERROR_NODE createMorpheme(_BLANK)
void yyerror(const char *s);
void my_yyerror(const char* msg);

int error_line = -1;
int pre_error_line = -1;
int syntax_correct = 1;

%}
/*定义类型*/

/*定义tokens*/
%token INT
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS MINUS STAR DIV
%token AND OR NOT
%token DOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
/*定义非终结符号*/

/*结合规则*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left DOT LP RP LB RB


/*定义产生式规则*/
%%
Program : ExtDefList {$$=createMorpheme(_Program);nodeGrowth($$,1,$1);root=$$;}
    ;
ExtDefList : /*empty*/ {$$=createMorpheme(_ExtDefList);nodeGrowth($$,1,ERROR_NODE);}
    | ExtDef ExtDefList {$$=createMorpheme(_ExtDefList);nodeGrowth($$, 2, $1, $2);}
    ;
ExtDef : Specifier ExtDecList SEMI {$$=createMorpheme(_ExtDef);nodeGrowth($$, 3, $1, $2, $3);}
    | Specifier SEMI   {$$=createMorpheme(_ExtDef); nodeGrowth($$, 2, $1, $2);}
    | Specifier FunDec CompSt   {$$=createMorpheme(_ExtDef); nodeGrowth($$, 3, $1, $2, $3);}
    | Specifier error {$$=createMorpheme(_ExtDef); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("missing \';\'");}
    ;
ExtDecList : VarDec {$$=createMorpheme(_ExtDecList); nodeGrowth($$, 1, $1);}
    | VarDec COMMA ExtDecList   {$$=createMorpheme(_ExtDecList); nodeGrowth($$, 3, $1, $2, $3);}
    ;
Specifier : TYPE    {$$=createMorpheme(_Specifier); nodeGrowth($$, 1, $1);}
    | StructSpecifier   {$$=createMorpheme(_Specifier); nodeGrowth($$, 1, $1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   {$$=createMorpheme(_StructSpecifier); nodeGrowth($$, 5, $1, $2, $3, $4, $5);}
    | STRUCT Tag    {$$=createMorpheme(_StructSpecifier); nodeGrowth($$, 2, $1, $2);}
    ;
OptTag : /*empty*/  {$$=createMorpheme(_OptTag); nodeGrowth($$, 1, ERROR_NODE);}
    | ID    {$$=createMorpheme(_OptTag); nodeGrowth($$, 1, $1);}
    ;
Tag : ID    {$$=createMorpheme(_Tag); nodeGrowth($$, 1, $1);}
    ;
VarDec : ID {$$=createMorpheme(_VarDec); nodeGrowth($$, 1, $1);}
    | VarDec LB INT RB  {$$=createMorpheme(_VarDec); nodeGrowth($$, 4, $1, $2, $3, $4);}
    | VarDec LB error RB {$$=createMorpheme(_VarDec); nodeGrowth($$, 4, $1, $2, ERROR_NODE, $4); error_line = $3->lineNumber; my_yyerror("something error between \'[]\'");}
    ;
FunDec : ID LP VarList RP   {$$=createMorpheme(_FunDec); nodeGrowth($$, 4, $1, $2, $3, $4);}
    | ID LP RP  {$$=createMorpheme(_FunDec); nodeGrowth($$, 3, $1, $2, $3);}
    | ID error VarList RP {$$=createMorpheme(_FunDec); nodeGrowth($$, 4, $1, ERROR_NODE, $3, $4); error_line = $2->lineNumber; my_yyerror("missing \'(\'");}
    | ID LP VarList error {$$=createMorpheme(_FunDec); nodeGrowth($$, 4, $1, $2, $3, ERROR_NODE); error_line = $4->lineNumber; my_yyerror("missing \')\'");}
    ;
VarList : ParamDec COMMA VarList    {$$=createMorpheme(_VarList); nodeGrowth($$, 3, $1, $2, $3);}
    | ParamDec  {$$=createMorpheme(_VarList); nodeGrowth($$, 1, $1);}
    ;
ParamDec : Specifier VarDec {$$=createMorpheme(_ParamDec); nodeGrowth($$, 2, $1, $2);}
    ;
CompSt : LC DefList StmtList RC {$$=createMorpheme(_CompSt); nodeGrowth($$, 4, $1, $2, $3, $4);}
    | error DefList StmtList RC {$$=createMorpheme(_CompSt); nodeGrowth($$, 4, ERROR_NODE, $2, $3, $4); error_line = $1->lineNumber; my_yyerror("missing \'{\'");}
    | LC DefList StmtList error {$$=createMorpheme(_CompSt); nodeGrowth($$, 4, $1, $2, $3, ERROR_NODE); error_line = $4->lineNumber; my_yyerror("missing \'}\'");}
    ;
StmtList : /*empty*/    {$$=createMorpheme(_StmtList); nodeGrowth($$, 1, ERROR_NODE);}
    | Stmt StmtList {$$=createMorpheme(_StmtList); nodeGrowth($$, 2, $1, $2);}
    ;
Stmt : Exp SEMI {$$=createMorpheme(_Stmt); nodeGrowth($$, 2, $1, $2);}
    | Exp error {$$=createMorpheme(_Stmt); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("missing \';\'");}
    | error SEMI {$$=createMorpheme(_Stmt); nodeGrowth($$, 2, ERROR_NODE, $2); error_line = $1->lineNumber; my_yyerror("something wrong with your expression");}
    | CompSt    {$$=createMorpheme(_Stmt); nodeGrowth($$, 1, $1);}
    | RETURN Exp SEMI   {$$=createMorpheme(_Stmt); nodeGrowth($$, 3, $1, $2, $3);}
    | RETURN Exp error  {$$=createMorpheme(_Stmt); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("missing \';\'");} 

    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, $3, $4, $5);}
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, ERROR_NODE, $4, $5); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | IF LP Exp RP error %prec LOWER_THAN_ELSE {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, $3, $4, ERROR_NODE); error_line = $5->lineNumber; my_yyerror("something wrong with your expression");}
   
    | IF LP Exp RP Stmt ELSE Stmt   {$$=createMorpheme(_Stmt); nodeGrowth($$, 7, $1, $2, $3, $4, $5, $6, $7);}
    | IF LP error RP Stmt ELSE Stmt {$$=createMorpheme(_Stmt); nodeGrowth($$, 7, $1, $2, ERROR_NODE, $4, $5, $6, $7); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | IF LP Exp RP error ELSE Stmt {$$=createMorpheme(_Stmt); nodeGrowth($$, 7, $1, $2, $3, $4, ERROR_NODE, $6, $7); error_line = $5->lineNumber; my_yyerror("something wrong with your expression");}
    | IF LP Exp RP Stmt ELSE error {$$=createMorpheme(_Stmt); nodeGrowth($$, 7, $1, $2, $3, $4, $5, $6, ERROR_NODE); error_line = $7->lineNumber; my_yyerror("something wrong with your expression");}

    | WHILE LP Exp RP Stmt  {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, $3, $4, $5);}
    | WHILE LP error RP Stmt {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, ERROR_NODE, $4, $5); error_line = $3->lineNumber; my_yyerror("error 13");}
    | WHILE LP Exp RP error {$$=createMorpheme(_Stmt); nodeGrowth($$, 5, $1, $2, $3, $4, ERROR_NODE); error_line = $5->lineNumber; my_yyerror("error 14");}
    ;
DefList : /*empty*/ {$$=createMorpheme(_DefList); nodeGrowth($$, 1, ERROR_NODE);}
    | Def DefList   {$$=createMorpheme(_DefList); nodeGrowth($$, 2, $1, $2);}
    ;
Def : Specifier DecList SEMI    {$$=createMorpheme(_Def); nodeGrowth($$, 3, $1, $2, $3);}
    | Specifier error SEMI {$$=createMorpheme(_Def); nodeGrowth($$, 3, $1, ERROR_NODE, $3); error_line = $2->lineNumber; my_yyerror("declarators definition error.");}
    | error SEMI {$$=createMorpheme(_Def); nodeGrowth($$, 2, ERROR_NODE, $2); error_line = $1->lineNumber; my_yyerror("definition error.");}
    ;
DecList : Dec   {$$=createMorpheme(_DecList); nodeGrowth($$, 1, $1);}
    | Dec COMMA DecList {$$=createMorpheme(_DecList); nodeGrowth($$, 3, $1, $2, $3);}
    | error COMMA DecList {$$=createMorpheme(_DecList); nodeGrowth($$, 3, ERROR_NODE, $2, $3); error_line = $1->lineNumber; my_yyerror("error 16");}
    ;
Dec : VarDec    {$$=createMorpheme(_Dec); nodeGrowth($$, 1, $1);}
    | VarDec ASSIGNOP Exp   {$$=createMorpheme(_Dec); nodeGrowth($$, 3, $1, $2, $3);}
    | error ASSIGNOP Exp {$$=createMorpheme(_Dec); nodeGrowth($$, 3, ERROR_NODE, $2, $3); error_line = $1->lineNumber; my_yyerror("error 17");}
    ;
Exp : Exp ASSIGNOP Exp  {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp AND Exp   {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp OR Exp    {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp RELOP Exp {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp PLUS Exp  {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp MINUS Exp {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp STAR Exp  {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp DIV Exp   {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | LP Exp RP {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | MINUS Exp {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, $2);}
    | NOT Exp   {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, $2);}
    | ID LP Args RP {$$=createMorpheme(_Exp); nodeGrowth($$, 4, $1, $2, $3, $4);}
    | ID LP RP  {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp LB Exp RB {$$=createMorpheme(_Exp); nodeGrowth($$, 4, $1, $2, $3, $4);}
    | Exp DOT ID    {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, $3);}
    | ID    {$$=createMorpheme(_Exp); nodeGrowth($$, 1, $1);}
    | INT   {$$=createMorpheme(_Exp); nodeGrowth($$, 1, $1);}
    | FLOAT {$$=createMorpheme(_Exp); nodeGrowth($$, 1, $1);}

    | Exp ASSIGNOP error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp AND error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp OR error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp RELOP error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp PLUS error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp MINUS error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp STAR error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | Exp DIV error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("something wrong with your expression");}
    | MINUS error {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("something wrong with your expression");}
    | NOT error {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("something wrong with your expression");}
    | error RP {$$=createMorpheme(_Exp); nodeGrowth($$, 2, ERROR_NODE, $2); error_line = $1->lineNumber; my_yyerror("missing \'(\'");}
    | LP error {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("missing \')\'");}
    | Exp LB error RB {$$=createMorpheme(_Exp); nodeGrowth($$, 4, $1, $2, ERROR_NODE, $4); error_line = $3->lineNumber; my_yyerror("something wrong between \'[]\'");}
    | error LB Exp RB {$$=createMorpheme(_Exp); nodeGrowth($$, 4, ERROR_NODE, $2, $3, $4); error_line = $1->lineNumber; my_yyerror("expression definition error");}
    | ID error  {$$=createMorpheme(_Exp); nodeGrowth($$, 2, $1, ERROR_NODE); error_line = $2->lineNumber; my_yyerror("something wrong with your expression");}
    | ID LP error {$$=createMorpheme(_Exp); nodeGrowth($$, 3, $1, $2, ERROR_NODE); error_line = $3->lineNumber; my_yyerror("missing \')\'");}
    ;
Args : Exp COMMA Args   {$$=createMorpheme(_Args); nodeGrowth($$, 3, $1, $2, $3);}
    | Exp   {$$=createMorpheme(_Args); nodeGrowth($$, 1, $1);}
    ;

%%

void  yyerror(const char* msg) {
    //printf("Errir type B at line %d : %s\n", error_line, msg);
    ;//do nothing
}

void my_yyerror(const char* msg) {
    if(error_line == pre_error_line) {
        return;
    }
    printf("Error type B at Line %d : %s\n", error_line, msg);
    pre_error_line = error_line;
    syntax_correct = 0;
}
