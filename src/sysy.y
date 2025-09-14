// 插入代码
%code requires{
    #include <memory>
    #include <string>
    #include <cstdio>
    #include "ast.hpp"
}

%{
#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char* s);
using namespace std;
%}

// Bison指令：定义语法分析器的配置和行为
%parse-param {std::unique_ptr<BaseAST> &ast}

%union {
    std::string *str_val;
    int int_val;
    BaseAST *ast_val;
}

// Token和类型声明：定义词法语法分析器的通信协议
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt
%type <int_val> Number

// 语法规则
%%
CompUnit
    :FuncDef{
        auto comp_unit = make_unique<CompUnitAST>();
        comp_unit->func_def = unique_ptr<BaseAST>($1);
        ast = std::move(comp_unit);
    }
    ;

FuncDef
    :FuncType IDENT '(' ')' Block {
        auto func_def = make_unique<FuncDefAST>();
        func_def->func_type = unique_ptr<BaseAST>($1);
        func_def->ident = *unique_ptr<string>($2);
        func_def->block = unique_ptr<BaseAST>($5);
        $$ = func_def.release();
    }
    ;

FuncType
    :INT {
        auto func_type = make_unique<FuncTypeAST>();
        $$ = func_type.release();
    }
    ;

Block
    :'{' Stmt '}'{
        auto block = make_unique<BlockAST>();
        block->stmt = unique_ptr<BaseAST>($2);
        $$ = block.release();
    }
    ;

Stmt
    :RETURN Number ';'{
        auto stmt = make_unique<StmtAST>();
        stmt->number = $2;
        $$ = stmt.release();
    }
    ;

Number
    :INT_CONST{
        $$ = $1;
    }
    ;

// 额外插入辅助函数
%%
void yyerror(unique_ptr<BaseAST>&ast, const char* s){
    extern int yylineno;
    extern char* yytext;
    fprintf(stderr, "ERROR: %s at '%s' on line %d\n", s, yytext, yylineno);
    ast.reset();
}