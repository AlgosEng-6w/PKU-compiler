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
    char char_val;
    BaseAST *ast_val;
}

// Token和类型声明：定义词法语法分析器的通信协议
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <char_val> UnaryOp AddOp
%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp PrimaryExp AddExp MulExp

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

// Stmt ::= "return" Exp ";";
Stmt
    :RETURN Exp ';'{
        auto stmt = make_unique<StmtAST>();
        stmt->exp = unique_ptr<BaseAST>($2);
        $$ = stmt.release();
    }
    ;

// Exp:: = AddExp;
Exp
    : AddExp {
        auto exp = make_unique<ExpAST>();
        exp->addexp = unique_ptr<BaseAST>($1);
        $$ = exp.release();
    }
    ;

// PrimaryExp ::= "(" Exp ")" | Number;
PrimaryExp
    : '(' Exp ')' {
        auto primaryexp = make_unique<PrimaryExpAST>();
        primaryexp->type = 1;
        primaryexp->exp = unique_ptr<BaseAST>($2);
        $$ = primaryexp.release();
    }
    | INT_CONST{
        auto primaryexp = make_unique<PrimaryExpAST>();
        primaryexp->type = 2;
        primaryexp->number = $1;
        $$ = primaryexp.release();
    }

UnaryExp
    : PrimaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 1;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($1);
        $$ = unaryexp.release();
    }
    | UnaryOp UnaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 2;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($2);
        unaryexp->unaryop = $1;
        $$ = unaryexp.release();
    }
    ;

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
AddExp
    : MulExp {
        auto addexp = make_unique<AddExpAST>();
        addexp->type = 1;
        addexp->mulexp = unique_ptr<BaseAST>($1);
        $$ = addexp.release();
    }
     | AddExp '+' MulExp {
        auto addexp = make_unique<AddExpAST>();
        addexp->type = 2;
        addexp->addexp = unique_ptr<BaseAST>($1);
        addexp->addop = '+'; 
        addexp->mulexp = unique_ptr<BaseAST>($3);
        $$ = addexp.release();
    }
    | AddExp '-' MulExp {
        auto addexp = make_unique<AddExpAST>();
        addexp->type = 3;
        addexp->addexp = unique_ptr<BaseAST>($1);
        addexp->addop = '-';
        addexp->mulexp = unique_ptr<BaseAST>($3);
        $$ = addexp.release();
    }
    ;

// MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
MulExp
    : UnaryExp{
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 1;
        mulexp->unaryexp = unique_ptr<BaseAST>($1);
        $$ = mulexp.release();
    }
    | MulExp '*' UnaryExp {
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 2;
        mulexp->mulexp = unique_ptr<BaseAST>($1);
        mulexp->mulop = '*';
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    | MulExp '/' UnaryExp {
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 3;
        mulexp->mulexp = unique_ptr<BaseAST>($1);
        mulexp->mulop = '/';
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    | MulExp '%' UnaryExp {
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 4;
        mulexp->mulexp = unique_ptr<BaseAST>($1);
        mulexp->mulop = '%';
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    ;
    
// UnaryOp ::= "+" | "-" | "!";
UnaryOp
    :'!' {
        $$ = '!';
    }
    | '+'{
        $$ = '+';
    }
    | '-'{
        $$ = '-';
    }
    ;

// AddOp ::= '+' | '-'
AddOp
    :'+' {
        $$ = '+';
    }
    | '-' {
        $$ = '-';
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