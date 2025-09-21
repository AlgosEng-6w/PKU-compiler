#pragma once
#include<memory>
#include<string>
#include<iostream>
#include<stdbool.h>
#include<vector>
#include<map>
#include<assert.h>

static int koopacnt;   // 临时变量计数器
static std::map<std::string, std::int32_t> constMap;

struct ExprResult {
    bool is_constant;
    int value;  // 如果是常量则存储常量值，否则存储临时变量编号
    
    ExprResult(bool is_const = false, int val = 0) : is_constant(is_const), value(val) {}
};

typedef enum {
    MUL_OP,     //  *
    DIV_OP,     // /
    MOD_OP      // %
} mulop_t;

typedef enum {
    UNARY_PLUS,     // +
    UNARY_MINUS,    // -
    UNARY_NOT       // !
} unaryop_t;

typedef enum {
    REL_LT,     // <
    REL_GT,     // >
    REL_LE,     // <=
    REL_GE,     // >=
} relop_t;


typedef enum{
    REL_EQ,     // ==
    REL_NE      // !=
} eqop_t;

typedef enum {
    LAND_OP,    // &&
    LOR_OP      // ||
} logicop_t;

typedef enum {
    ADD_OP,     // +
    SUB_OP      // -
} addop_t;

class BaseAST {
    public:
        virtual ~BaseAST() = default;
        virtual void Dump() const = 0;
        virtual ExprResult KoopaIR() const = 0;
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> func_def;

        void Dump() const override {
            std::cout << "CompUnitAST { ";
            func_def->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            func_def->KoopaIR();
            return ExprResult();
        }
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> func_type;
        std::string ident;
        std::unique_ptr<BaseAST> block; 

        void Dump() const override{
            std::cout << "FuncDefAST { ";
            func_type->Dump();
            std::cout << ", " << ident << ", ";
            block->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            std::cout << "fun @" << ident << "(): ";
            func_type->KoopaIR();
            std::cout << "{ " << std::endl;
            block->KoopaIR();
            std::cout << " }" << std::endl;   
            return ExprResult();
        }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST{
    public:
        void Dump() const override{
            std::cout << "FuncTypeAST { int }";
        }
        
        ExprResult KoopaIR() const override{
            std::cout << "i32" << std::endl;
            return ExprResult();
        };
};

// Block ::= "{" {BlockItem} "}";
class BlockAST : public BaseAST{
    public:
        std::vector<std::unique_ptr<BaseAST>> blockitem_list;

        void Dump() const override{
            std::cout << "BlockAST { ";
            for (const auto& blockitem : blockitem_list){
                blockitem->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            std::cout << "%entry:" << std::endl;
            for (const auto& blockitem : blockitem_list){
                blockitem->KoopaIR();
            }
            return ExprResult();
        }
};

// BlockItem ::= Decl | Stmt;
class BlockItemAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> decl_stmt;

        void Dump() const override {
            std::cout << "BlockItemAST { ";
            decl_stmt->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            return decl_stmt->KoopaIR();
        }
};

// Stmt ::= "return" Exp ";";
class StmtAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> exp;

        void Dump() const override{
            std::cout << "StmtAST { ";
            exp->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            ExprResult result = exp->KoopaIR();
            std::cout << "  ret ";
            if (result.is_constant) {
                std::cout << result.value;
            } else {
                std::cout << " %" <<result.value;
            }
            std::cout << std::endl;
            return ExprResult();
        }
};

// Exp ::= LOrExp;
class ExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> lorexp;

        void Dump() const override{
            std::cout << "ExpAST { ";
            lorexp->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            return lorexp->KoopaIR();
        }
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
// Number ::= INT_CONST;
class PrimaryExpAST : public BaseAST{
    public:
        int type;
        std::int32_t number;
        std::unique_ptr<BaseAST> exp_lval;

        void Dump() const override{
            std::cout << "PrimaryExpAST { ";
            if (type == 1) exp_lval->Dump();
            else if (type == 2) std::cout << "Number: " << number;
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return exp_lval->KoopaIR();
            else if (type == 2){
                return ExprResult(true, number);
            }
            return ExprResult();
        }
};

// LVal ::= IDEDNT;
class LValAST : public BaseAST{
    public:
        std::string ident;

        void Dump() const override {
            std::cout << "LValAST { " << ident << " }";
        }

        ExprResult KoopaIR() const override {
            if (constMap.find(ident) != constMap.end()) {
                return ExprResult(true, constMap[ident]);
            }
            else assert(false);
            return ExprResult();        
        }
};

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
class AddExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> mulexp;
        std::unique_ptr<BaseAST> addexp;
        addop_t addop;

        void Dump() const override{
            std::cout << "AddExpAST { ";
            if (type == 1) mulexp->Dump();
            else if (type == 2){
                addexp->Dump();
                std::cout << "addop ";
                mulexp->Dump();
            }
            std::cout << " }";
            
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return mulexp->KoopaIR();
            else if (type == 2) {
                ExprResult left = addexp->KoopaIR();
                ExprResult right = mulexp->KoopaIR();

                if (left.is_constant && right.is_constant){
                    switch (addop) {
                        case ADD_OP: 
                            return ExprResult(true, left.value+right.value);
                            break;
                        case SUB_OP: 
                            return ExprResult(true, left.value-right.value);
                            break;
                    }
                }

                std::cout << "  %" << koopacnt << " = ";
                switch(addop) {
                    case ADD_OP: std::cout << "add "; break;
                    case SUB_OP: std::cout << "sub "; break;
                    
                }
                if (left.is_constant) std::cout << left.value;
                else std::cout << "%" << left.value;
                std::cout << ", ";
                if (right.is_constant) std::cout << right.value;
                else std::cout << "%" << right.value;
                std::cout << std::endl;
                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};

// MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
class MulExpAST : public BaseAST {
    public:
        int type;
        std::unique_ptr<BaseAST> unaryexp;
        std::unique_ptr<BaseAST> mulexp;
        mulop_t mulop;

        void Dump() const override{
            std::cout << "MulExp { ";
            if (type == 1) unaryexp->Dump();
            else if (type == 2) {
                mulexp->Dump();
                std::cout << " " << mulop << " ";
                unaryexp->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            if (type == 1) return unaryexp->KoopaIR();
            
            ExprResult left = mulexp->KoopaIR();
            ExprResult right = unaryexp->KoopaIR();

            if (left.is_constant && right.is_constant){
                switch (mulop) {
                    case MUL_OP: 
                        return ExprResult(true, left.value*right.value);
                        break;
                    case DIV_OP: 
                        return ExprResult(true, left.value/right.value);
                        break;
                    case MOD_OP: 
                        return ExprResult(true, left.value%right.value);
                        break;
                }
            }


            std::cout << "  %" << koopacnt << " = ";

            switch(mulop){
                case MUL_OP: std::cout << "mul "; break;
                case DIV_OP: std::cout << "div "; break;
                case MOD_OP: std::cout << "mod "; break;
            }

            if (left.is_constant) std::cout << left.value;
            else std::cout << "%" << left.value;
            std::cout << ", ";

            if (right.is_constant) std::cout << right.value;
            else std::cout << "%" << right.value;
            std::cout << std::endl;

            return ExprResult(false, koopacnt++);
        }
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> primaryexp_unaryexp;
        unaryop_t unaryop;
        int type;
        
        void Dump() const override{
            std::cout << "UnaryExpAST { ";
            if (type == 1) primaryexp_unaryexp->Dump();
            else if (type == 2) {
                std::cout << unaryop << " ";
                primaryexp_unaryexp->Dump();
            }
            
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return primaryexp_unaryexp->KoopaIR();
            else if (type == 2){
                ExprResult operand = primaryexp_unaryexp->KoopaIR();
                if (unaryop == UNARY_PLUS) return operand;
                std::cout <<  "  %" << koopacnt << " = ";
                
                switch(unaryop) {
                    case UNARY_PLUS: break;
                    case UNARY_MINUS:
                        std::cout << "sub 0, ";
                        break;
                    case UNARY_NOT:
                        std::cout << "eq 0, ";
                }

                if (operand.is_constant) {
                    std::cout << operand.value;
                } else {
                    std::cout << "%" << operand.value;
                }
                std::cout << std::endl;
                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};


// RelExp ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
class RelExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> addexp;
        std::unique_ptr<BaseAST> relexp;
        relop_t relop;

        void Dump() const override{
            std::cout << "RelExpAST { ";
            if (type == 1) addexp->Dump();
            else if (type == 2){
                relexp->Dump();
                std::cout << " relop ";
                addexp->Dump();
            }
            std::cout << " }";
        }
        
        ExprResult KoopaIR() const override{
            if (type == 1) return addexp->KoopaIR();
            else if (type == 2) {
                ExprResult left = relexp->KoopaIR();
                ExprResult right = addexp->KoopaIR();
                std::cout << "  %" << koopacnt << " = ";
                switch(relop){
                    case REL_LT: std::cout << "lt "; break;
                    case REL_GT: std::cout << "gt "; break;
                    case REL_LE: std::cout << "le "; break;
                    case REL_GE: std::cout << "ge "; break;
                }
                if (left.is_constant) std::cout << left.value << ", ";
                else std::cout << "%" << left.value << ", ";
                if (right.is_constant) std::cout << right.value << std::endl;
                else std::cout << "%" << right.value << std::endl;
                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};

// EqExp ::= RelExp | EqExp ("==" | "!=") RelExp;
class EqExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> relexp;
        std::unique_ptr<BaseAST> eqexp;
        eqop_t eqop;

        void Dump() const override{
            std::cout << "EqExpAST { ";
            if (type == 1) relexp->Dump();
            else if (type == 2) {
                eqexp->Dump();
                std::cout << "relop";
                relexp->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return relexp->KoopaIR();
            else if (type == 2){
                ExprResult left = eqexp->KoopaIR();
                ExprResult right = relexp->KoopaIR();
                std::cout << "  %" << koopacnt << " = ";
                switch (eqop) {
                    case REL_EQ: std::cout << "eq "; break;
                    case REL_NE: std::cout << "ne "; break;
                }
                if (left.is_constant) std::cout << left.value << ", ";
                else std::cout << "%" << left.value << ", ";
                if (right.is_constant) std::cout << right.value << std::endl;
                else std::cout << "%" << right.value << std::endl;

                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> eqexp;
        std::unique_ptr<BaseAST> landexp;
        logicop_t logicop;

        void Dump() const override{
            std::cout << "LAndExpAST { ";
            if (type == 1) eqexp->Dump();
            else if (type == 2) {
                landexp->Dump();
                std::cout << "logicop";
                eqexp->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return eqexp->KoopaIR();
            else if (type == 2) {
                ExprResult left = landexp->KoopaIR();
                ExprResult right = eqexp->KoopaIR();
                std::cout << "  %" << koopacnt << " = ";
                
                if (left.is_constant) std::cout << left.value << " && ";
                else std::cout << "%" << left.value << " && ";
                if (right.is_constant) std::cout << right.value << std::endl;
                else std::cout << "%" << right.value << std::endl;

                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> lorexp;
        std::unique_ptr<BaseAST> landexp;
        logicop_t logicop;
        
        void Dump() const override{
            std::cout << "LOrExpAST { ";
            if (type == 1) landexp->Dump();
            else if (type == 2) {
                lorexp->Dump();
                std::cout << "logicop";
                landexp->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return landexp->KoopaIR();
            else if (type == 2) {
                ExprResult left = lorexp->KoopaIR();
                ExprResult right = landexp->KoopaIR();
                std::cout << "  %" << koopacnt << " = ";
                
                if (left.is_constant) std::cout << left.value << " || ";
                else std::cout << "%" << left.value << " || ";
                if (right.is_constant) std::cout << right.value << std::endl;
                else std::cout << "%" << right.value << std::endl;

                return ExprResult(false, koopacnt++);
            }
            return ExprResult();
        }
};

// Decl ::= ConstDecl;
class DeclAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> constdecl;

        void Dump() const override{
            std::cout << "DeclAST { ";
            constdecl->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            return constdecl->KoopaIR();
        }
};

// ConstDecl ::= "const" BType ConstDef {"," ConstDef} ";";
class ConstDeclAST : public BaseAST {
    public:
        std::unique_ptr<BaseAST> btype;
        std::vector<std::unique_ptr<BaseAST>> constdef_list;

        void Dump() const override {
            std::cout << "ConstDecl { ";
            btype->Dump();
            for (const auto& constdef : constdef_list) {
                constdef->Dump();
            }
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            btype->KoopaIR();
            for (const auto& constdef : constdef_list) {
                constdef->KoopaIR();
            }
            return ExprResult();
        }
};

// BType ::= "int";
class BTypeAST : public BaseAST{
    public:
        void Dump() const override{
            std::cout << "BTypeAST { int }";
        }

        ExprResult KoopaIR() const override {
            return ExprResult();
        }
};

// ConstDef ::= IDENT "=" ConstInitVal;
class ConstDefAST : public BaseAST {
    public:
        std::string ident;
        std::unique_ptr<BaseAST> constintval;

        void Dump() const override {
            std::cout << "ConstDefAST { ";
            std::cout << "IDENT = " << ident << ", value = ";
            constintval->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            ExprResult intval = constintval->KoopaIR();
            if (intval.is_constant){
                int num = intval.value;
                constMap[ident] = num;
            }
            else assert(false);
            return ExprResult();
        }
};

// ConstInitVal ::= ConstExp;
class ConstInitValAST : public BaseAST {
    public:
        std::unique_ptr<BaseAST> constexp;

        void Dump() const override {
            std::cout << "ConstInitValAST { ";
            constexp->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            return constexp->KoopaIR();
        }
};

// ConstExp ::= Exp
class ConstExpAST : public BaseAST {
    public:
        std::unique_ptr<BaseAST> exp;

        void Dump() const override{
            std::cout << "ConstExpAST { ";
            exp->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override {
            return exp->KoopaIR();
        }
};