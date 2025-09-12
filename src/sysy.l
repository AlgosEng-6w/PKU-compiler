// 可定义一些额外选项
%option noyywrap
%option nounput
%option noinput

%{
// 嵌入代码到词法分析器头部，定义必要的头文件
#include <cstdlib>
#include <string>
#include "sysy.tab.hpp"
using namespace std;    
%}

// 正则表达式定义宏部分
WhiteSpace      [ \t\n]*
LineComment     \/\/[.]*
BLockComment    \/\*[\s\S]*?\*\/
Identifier      [a-zA-Z_][a-zA-Z_0-9]*
Decimal         [1-9][0-9]*
Octal           0[0-7]*
Hexadecimal     0[xX][0-9a-fA-F]+
%%

// 模式匹配规则部分
{WhiteSpace}    {}
{LineComment}   {}
{BLockComment}  {}
"int"           {return INT;}
"return"        {return RETURN;}

{Identifier}    {yylval.str_val = new string(yytext); return IDENT;}
// 1. 堆上建立新的string对象
// 2. 复制yytext中的内容到新对象
// 3. 将指针存储在yylval中传入语法分析器

// 值类型，不用使用new，直接存入联合体中
{Decimal}       {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}
{Octal}         {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}
{Hexadecimal}   {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}

.               {return yytext[0];}
%%

// 嵌入辅助函数到词法分析器尾部，可在main.c中显式调用
