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
    std::vector<std::unique_ptr<BaseAST>>* ast_list_ptr;
}

// Token和类型声明：定义词法语法分析器的通信协议
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST
%token LE GE EQ NE LAND LOR
%token CONST

%type <ast_val> FuncDef FuncType Block Stmt UnaryExp PrimaryExp AddExp 
%type <ast_val> LAndExp LOrExp MulExp Exp RelExp EqExp VarDecl VarDef InitVal
%type <ast_val> Decl ConstDecl BType ConstDef ConstInitVal BlockItem ConstExp LVal
%type <ast_list_ptr> BlockItemList ConstDefList VarDefList

// 语法规则
%%
// CompUnit ::= FuncDef;
CompUnit
    :FuncDef{
        auto comp_unit = make_unique<CompUnitAST>();
        comp_unit->func_def = unique_ptr<BaseAST>($1);
        ast = std::move(comp_unit);
    }
    ;

// FuncDef ::= FuncType IDENT "(" ")" Block;
FuncDef
    :FuncType IDENT '(' ')' Block {
        auto func_def = make_unique<FuncDefAST>();
        func_def->func_type = unique_ptr<BaseAST>($1);
        func_def->ident = *unique_ptr<string>($2);
        func_def->block = unique_ptr<BaseAST>($5);
        $$ = func_def.release();
    }
    ;

// FuncType ::= "int";
FuncType
    :INT {
        auto func_type = make_unique<FuncTypeAST>();
        $$ = func_type.release();
    }
    ;

// Block ::= "{" {BlockItem} "}";
Block
    : '{' '}' {
        auto block = make_unique<BlockAST>();
        $$ = block.release();
    }
    |'{' BlockItemList '}'{
        auto block = make_unique<BlockAST>();
        block->blockitem_list = move(*($2));
        delete $2;
        $$ = block.release();
    }
    ;

BlockItemList
    : BlockItem {
        auto list = new vector<unique_ptr<BaseAST>>();
        list->push_back(unique_ptr<BaseAST>($1));
        $$ = list;
    }
    | BlockItemList BlockItem {
        $1->push_back(unique_ptr<BaseAST>($2));
        $$ = $1;
    }
    ;

// BlockItem ::= Decl | Stmt;
BlockItem
    :Decl {
        auto blockitem = make_unique<BlockItemAST>();
        blockitem->decl_stmt = unique_ptr<BaseAST>($1);
        $$ = blockitem.release();
    }
    |Stmt {
        auto blockitem = make_unique<BlockItemAST>();
        blockitem->decl_stmt = unique_ptr<BaseAST>($1);
        $$ = blockitem.release();
    }
    ;

// Decl ::= ConstDecl | VarDecl;
Decl
    : ConstDecl {
        auto decl = make_unique<DeclAST>();
        decl->const_vardecl = std::unique_ptr<BaseAST>($1);
        $$ = decl.release();
    }
    | VarDecl {
        auto decl = make_unique<DeclAST>();
        decl->const_vardecl = std::unique_ptr<BaseAST>($1);
        $$ = decl.release();
    }

// ConstDecl ::= "const" BType ConstDef {"," ConstDef} ";";
ConstDecl
    : CONST BType ConstDefList ';' {
        auto constdecl = make_unique<ConstDeclAST>();
        constdecl->btype = unique_ptr<BaseAST>($2);
        constdecl->constdef_list = move(*($3));
        delete $3;
        $$ = constdecl.release();
    }
    ;

ConstDefList
    : ConstDef {
        auto list = new vector<unique_ptr<BaseAST>>();
        list->push_back(unique_ptr<BaseAST>($1));
        $$ = list;
    }
    | ConstDefList ',' ConstDef {
        $1->push_back(unique_ptr<BaseAST>($3));
        $$ = $1;
    }
    ;

// ConstDef ::= IDENT "=" ConstInitVal;
ConstDef 
    : IDENT '=' ConstInitVal{
        auto constdef = make_unique<ConstDefAST>();
        constdef->ident = *unique_ptr<string>($1);
        constdef->constintval = unique_ptr<BaseAST>($3);
        $$ = constdef.release();
    }
    ;

// ConstInitVal ::= ConstExp;
ConstInitVal
    : ConstExp{
        auto constinitval = make_unique<ConstInitValAST>();
        constinitval->constexp = unique_ptr<BaseAST>($1);
        $$ = constinitval.release();
    }
    ;

// ConstExp ::= Exp
ConstExp
    : Exp{
        auto constexp = make_unique<ConstExpAST>();
        constexp->exp = unique_ptr<BaseAST>($1);
        $$ = constexp.release();
    }
    ;

// VarDecl ::= BType VarDef {"," VarDef} ";";
VarDecl
    : BType VarDefList ';'{
        auto vardecl = make_unique<VarDeclAST>();
        vardecl->btype = unique_ptr<BaseAST>($1);
        vardecl->vardef_list = move(*($2));
        delete($2);
        $$ = vardecl.release();
    }
    ;

VarDefList
    : VarDef {
        auto list = new vector<unique_ptr<BaseAST>>();
        list->push_back(unique_ptr<BaseAST>($1));
        $$ = list;
    }
    | VarDefList ',' VarDef {
        $1->push_back(unique_ptr<BaseAST>($3));
        $$ = $1;
    }
    ;

// VarDef ::= IDENT | IDENT "=" InitVal;
VarDef
    : IDENT {
        auto vardef = make_unique<VarDefAST>();
        vardef->ident = *unique_ptr<string>($1);
        vardef.release();
    }
    | IDENT '=' InitVal {
        auto vardef = make_unique<VarDefAST>();
        vardef->ident = *unique_ptr<string>($1);
        vardef->initval = unique_ptr<BaseAST>($3);
        $$ = vardef.release();
    }
    ;

// InitVal ::= Exp;
InitVal
    : Exp {
        auto initval = make_unique<InitValAST>();
        initval->exp = unique_ptr<BaseAST>($1);
        $$ = initval.release();
    }
    ;

// BType ::= "int";
BType
    : INT {
        auto btype = make_unique<BTypeAST>();
        $$ = btype.release();
    }
    ;

// LVal ::= IDEDNT;
LVal
    : IDENT{
        auto lval = make_unique<LValAST>();
        lval->ident = *unique_ptr<string>($1);
        $$ = lval.release();
    }
    ;

// Stmt ::= LVal "=" Exp ";" | "return" Exp ";";
Stmt
    :LVal '=' Exp ';'{
        auto stmt = make_unique<StmtAST>();
        stmt->type = 1;
        stmt->lval = unique_ptr<BaseAST>($1);
        stmt->exp = unique_ptr<BaseAST>($3);
        $$ = stmt.release();
    }
    |RETURN Exp ';'{
        auto stmt = make_unique<StmtAST>();
        stmt->type = 2;
        stmt->exp = unique_ptr<BaseAST>($2);
        $$ = stmt.release();
    }
    ;

// Exp:: = LOrExp;
Exp
    : LOrExp {
        auto exp = make_unique<ExpAST>();
        exp->lorexp = unique_ptr<BaseAST>($1);
        $$ = exp.release();
    }
    ;


// LOrExp ::= LAndExp | LOrExp LOR LAndExp;
LOrExp
    : LAndExp {
        auto lorexp = make_unique<LOrExpAST>();
        lorexp->type = 1;
        lorexp->landexp = unique_ptr<BaseAST>($1);
        $$ = lorexp.release();
    }
    | LOrExp LOR LAndExp {
        auto lorexp = make_unique<LOrExpAST>();
        lorexp->type = 2;
        lorexp->lorexp = unique_ptr<BaseAST>($1);
        lorexp->logicop = LOR_OP;
        lorexp->landexp = unique_ptr<BaseAST>($3);
        $$ = lorexp.release();
    }
    ;

// LAndExp ::= EqExp | LAndExp LAND EqExp;
LAndExp
    : EqExp {
        auto landexp = make_unique<LAndExpAST>();
        landexp->type = 1;
        landexp->eqexp = unique_ptr<BaseAST>($1);
        $$ = landexp.release();
    }
    | LAndExp LAND EqExp {
        auto landexp = make_unique<LAndExpAST>();
        landexp->type = 2;
        landexp->landexp = unique_ptr<BaseAST>($1);
        landexp->logicop = LAND_OP;
        landexp->eqexp = unique_ptr<BaseAST>($3);
        $$ = landexp.release();
    }
    ;

// EqExp ::= RelExp | EqExp (EQ | NE) RelExp;
EqExp
    : RelExp {
        auto eqexp = make_unique<EqExpAST>();
        eqexp->type = 1;
        eqexp->relexp = unique_ptr<BaseAST>($1);
        $$ = eqexp.release();
    }
    | EqExp EQ RelExp {
        auto eqexp = make_unique<EqExpAST>();
        eqexp->type = 2;
        eqexp->eqexp = unique_ptr<BaseAST>($1);
        eqexp->eqop = REL_EQ;
        eqexp->relexp = unique_ptr<BaseAST>($3);
        $$ = eqexp.release();
    }
    | EqExp NE RelExp {
        auto eqexp = make_unique<EqExpAST>();
        eqexp->type = 2;
        eqexp->eqexp = unique_ptr<BaseAST>($1);
        eqexp->eqop = REL_NE;
        eqexp->relexp = unique_ptr<BaseAST>($3);
        $$ = eqexp.release();
    }
    ;

// RelExp ::= AddExp | RelExp ("<" | ">" | LE | GE) AddExp;
RelExp
    : AddExp {
        auto relexp = make_unique<RelExpAST>();
        relexp->type = 1;
        relexp->addexp = unique_ptr<BaseAST>($1);
        $$ = relexp.release();
    }
    | RelExp '<' AddExp {
        auto relexp = make_unique<RelExpAST>();
        relexp->type = 2;
        relexp->relexp = unique_ptr<BaseAST>($1);
        relexp->relop = REL_LT;
        relexp->addexp = unique_ptr<BaseAST>($3);
        $$ = relexp.release();
    }
    | RelExp '>' AddExp {
        auto relexp = make_unique<RelExpAST>();
        relexp->type = 2;
        relexp->relexp = unique_ptr<BaseAST>($1);
        relexp->relop = REL_GT;
        relexp->addexp = unique_ptr<BaseAST>($3);
        $$ = relexp.release();
    }
    | RelExp LE AddExp {
        auto relexp = make_unique<RelExpAST>();
        relexp->type = 2;
        relexp->relexp = unique_ptr<BaseAST>($1);
        relexp->relop = REL_LE;
        relexp->addexp = unique_ptr<BaseAST>($3);
        $$ = relexp.release();
    }
    | RelExp GE AddExp {
        auto relexp = make_unique<RelExpAST>();
        relexp->type = 2;
        relexp->relexp = unique_ptr<BaseAST>($1);
        relexp->relop = REL_GE;
        relexp->addexp = unique_ptr<BaseAST>($3);
        $$ = relexp.release();
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
        addexp->addop = ADD_OP; 
        addexp->mulexp = unique_ptr<BaseAST>($3);
        $$ = addexp.release();
    }
    | AddExp '-' MulExp {
        auto addexp = make_unique<AddExpAST>();
        addexp->type = 2;
        addexp->addexp = unique_ptr<BaseAST>($1);
        addexp->addop = SUB_OP;
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
        mulexp->mulop = MUL_OP;
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    | MulExp '/' UnaryExp {
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 2;
        mulexp->mulexp = unique_ptr<BaseAST>($1);
        mulexp->mulop = DIV_OP;
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    | MulExp '%' UnaryExp {
        auto mulexp = make_unique<MulExpAST>();
        mulexp->type = 2;
        mulexp->mulexp = unique_ptr<BaseAST>($1);
        mulexp->mulop = MOD_OP;
        mulexp->unaryexp = unique_ptr<BaseAST>($3);
        $$ = mulexp.release();
    }
    ;

// UnaryExp ::= PrimaryExp | ('-'|'+'|'!') UnaryExp;
UnaryExp
    : PrimaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 1;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($1);
        $$ = unaryexp.release();
    }
    | '+' UnaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 2;
        unaryexp->unaryop = UNARY_PLUS;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($2);
        $$ = unaryexp.release();
    }
    | '-' UnaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 2;
        unaryexp->unaryop = UNARY_MINUS;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($2);
        $$ = unaryexp.release();
    }
    | '!' UnaryExp {
        auto unaryexp = make_unique<UnaryExpAST>();
        unaryexp->type = 2;
        unaryexp->unaryop = UNARY_NOT;
        unaryexp->primaryexp_unaryexp = unique_ptr<BaseAST>($2);
        $$ = unaryexp.release();
    }
    ;

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
PrimaryExp
    : '(' Exp ')' {
        auto primaryexp = make_unique<PrimaryExpAST>();
        primaryexp->type = 1;
        primaryexp->exp_lval = unique_ptr<BaseAST>($2);
        $$ = primaryexp.release();
    }
    | LVal {
        auto primaryexp = make_unique<PrimaryExpAST>();
        primaryexp->type = 1;
        primaryexp->exp_lval = unique_ptr<BaseAST>($1);
        $$ = primaryexp.release();
    }
    | INT_CONST{
        auto primaryexp = make_unique<PrimaryExpAST>();
        primaryexp->type = 2;
        primaryexp->number = $1;
        $$ = primaryexp.release();
    }


// 额外插入辅助函数
%%
void yyerror(unique_ptr<BaseAST>&ast, const char* s){
    extern int yylineno;
    extern char* yytext;
    fprintf(stderr, "ERROR: %s at '%s' on line %d\n", s, yytext, yylineno);
    ast.reset();
}