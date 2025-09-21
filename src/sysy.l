%option noyywrap
%option nounput
%option noinput
%option yylineno

%{
#include <cstdlib>
#include <string>
#include "sysy.tab.hpp"
using namespace std;    
%}

WhiteSpace      [ \t\n\r]*
LineComment     \/\/.*$
BLockComment    "/*"[^*]*\*+([^*/][^*]*\*+)*\/
Identifier      [a-zA-Z_][a-zA-Z_0-9]*
Decimal         [1-9][0-9]*|0
Octal           0[0-7]+
Hexadecimal     0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    {}
{LineComment}   {}
{BLockComment}  {}
"int"           {return INT;}
"return"        {return RETURN;}
"const"         {return CONST;}

{Identifier}    {yylval.str_val = new string(yytext); return IDENT;}


{Decimal}       {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}
{Octal}         {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}
{Hexadecimal}   {yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST;}

"<="        { return LE; }
">="        { return GE; }
"=="        { return EQ; }
"!="        { return NE; }
"&&"        { return LAND; }
"||"        { return LOR; }

.               {return yytext[0];}
%%


