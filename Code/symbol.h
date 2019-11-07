#ifndef __SYMBOL_H__
#define __SYMBOL_H__
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//定义一个符号的类型
typedef enum SymbolTypes
{
    INT_SYMBOL = 1,
    FLOAT_SYMBOL,
    ARRAY_SYMBOL,
    STRUCT_TYPE_SYMBOL, //该符号是一个struct定义
    STRUCT_VAL_SYMBOL,  //该符号是一个struct实例
    FUNC_SYMBOL
} SymbolTypes;

//定义值可能的类型
typedef enum ValueTypes
{
    _INT_TYPE_ = 1,
    _FLOAT_TYPE_,
    _STRUCT_TYPE_,
    _ARRAY_TYPE_,
    //_POINTER_TYPE_, 没有定义指针运算符，不需要指针类型
} ValueTypes;

//INT_SYMBOL的内容
typedef struct IntContent
{
    int val;
} IntContent;

//FLOAT_SYMBOL的内容
typedef struct FloatContent
{
    float val;
} FloatContent;

//ARRAY_SYMBOL的内容
typedef struct ArrayContent
{
    ValueTypes type; //基类型
    int dimensions;  //维数
    int *size;       //每一维的大小
    char *typeName;  //如果基类型是一个struct，那么这个字段指明struct的名字
} ArrayContent;

//FUNC_SYMBOL的内容
typedef struct Argument
{
    char *name;            //参数名，cmm中所有变量的作用域都是全局的，所以形参也在符号表中，应去符号表里查找
    struct Argument *next; //下一个参数
} Argument;

typedef struct FuncContent
{
    ValueTypes retType;  //返回值类型
    char *typeName;      //如果返回值类型是一个struct，那么这个字段指明struct的名字       //返回值
    Argument *arguments; //参数列表
} FuncContent;

//STRUCT_DEF_SYMBOL的内容
typedef struct Field
{
    char *name;         //域名，cmm中所有变量的作用域都是全局的，所以field也在符号表中，应去符号表里查找
    struct Field *next; //下一个域
} Field;

typedef struct StructTypeContent
{
    Field *fields;
} StructTypeContent;

//STRUCT_VAL_SYMBOL的内容
typedef struct StructValueContent
{
    char *typeName; //指明这个value的类型是哪个struct
} StructValueContent;

//Expression的类型
typedef struct ExpType
{
    bool leftValue;
    ValueTypes type;
    /* 额外字段 */
    union {
        //如果type是
        char *typeName;
        struct ArrayContent *arrayContent;
    };
} ExpType;

//实参的类型
typedef struct ParaType
{
    ValueTypes type;
    union {
        char *typeName;
        ArrayContent *arrayContent;
    } ;
    struct ParaType *next;
} ParaType;

/*符号的定义
* 1. 符号名
* 2. 符号类型
* 3. 符号内容*/
typedef struct Symbol
{
    char *name;
    char* variable;
    SymbolTypes symbol_type;
    union {
        IntContent *int_content;
        FloatContent *float_content;
        FuncContent *func_content;
        ArrayContent *array_content;
        StructTypeContent *struct_def;
        StructValueContent *struct_value;
    };
    struct Symbol *next;
} Symbol;

Symbol *createSymbol();
bool setSymbolName(Symbol *s, char *name);
bool setSymbolVariable(Symbol *s, char *name);
bool overWriteSymbolVariable(Symbol *s, char *name);
bool setSymbolType(Symbol *s, SymbolTypes type);
bool addArrayDimension(Symbol *s, int size);
bool setArrayType(Symbol *s, ValueTypes type, char *name);
bool setFuncReturnValue(Symbol *s, ValueTypes type, char *name);
bool addFuncArgument(Symbol *s, char *name);
bool setStructValueType(Symbol *s, char *name);
bool addStructTypeField(Symbol *s, char *name);
char *valueTypesToString(ValueTypes type);

bool outputSymbol(Symbol *s);
bool expTpyeEqual(ExpType *t1, ExpType *t2);
bool structTypeEqual(StructTypeContent *s1, StructTypeContent *s2);
bool arrayTypeEqual(ArrayContent *a1, ArrayContent *a2, bool useLength);
bool isField(StructTypeContent *s, char *fieldName);
bool argsMatch(Argument* args, ParaType* parameters);

int calcSize(char* symbolName);
int calcFieldOffset(char* fieldName);


#endif
