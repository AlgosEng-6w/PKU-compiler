#pragma once
#include<memory>
#include<string>
#include<iostream>
#include<stdbool.h>

static int koopacnt;   // 临时变量计数器

struct ExprResult {
    bool is_constant;
    int value;  // 如果是常量则存储常量值，否则存储临时变量编号
    
    ExprResult(bool is_const = false, int val = 0) : is_constant(is_const), value(val) {}
};

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

// Block ::= "{" Stmt "}";
class BlockAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> stmt;

        void Dump() const override{
            std::cout << "BlockAST { ";
            stmt->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            std::cout << "%entry:" << std::endl;
            stmt->KoopaIR();
            return ExprResult();
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
            std::cout << "  ret";
            if (result.is_constant) {
                std::cout << result.value;
            } else {
                std::cout << " %" <<result.value;
            }
            std::cout << std::endl;
            return ExprResult();
        }
};

// Exp ::= AddExp;
class ExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> addexp;

        void Dump() const override{
            std::cout << "ExpAST { ";
            addexp->Dump();
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            return addexp->KoopaIR();
        }
};

// PrimaryExp ::= "(" Exp ")" | Number;
// Number ::= INT_CONST;
class PrimaryExpAST : public BaseAST{
    public:
        int type;
        std::int32_t number;
        std::unique_ptr<BaseAST> exp;

        void Dump() const override{
            std::cout << "PrimaryExpAST { ";
            if (type == 1) exp->Dump();
            else if (type == 2) std::cout << "Number: " << number;
            std::cout << " }";
        }

        ExprResult KoopaIR() const override{
            if (type == 1) return exp->KoopaIR();
            else if (type == 2){
                return ExprResult(true, number);
            }
            return ExprResult();
        }
};

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
class AddExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> mulexp;
        std::unique_ptr<BaseAST> addexp;
        char addop;

        void Dump() const override{
            std::cout << "AddExpAST { ";
            if (type == 1) mulexp->Dump();
            else if (type == 2 || type == 3){
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
                std::cout << "  %" << koopacnt << " = add ";
                if (left.is_constant) std::cout << left.value;
                else std::cout << "%" << left.value;
                std::cout << ", ";
                if (right.is_constant) std::cout << right.value;
                else std::cout << "%" << right.value;
                std::cout << std::endl;
                return ExprResult(false, koopacnt++);
            }
            else if (type == 3){
                ExprResult left = addexp->KoopaIR();
                ExprResult right = mulexp->KoopaIR();
                std::cout << "  %" << koopacnt << " = add ";
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
        char mulop;

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

            std::cout << "  %" << koopacnt << " = ";
            if (type == 2) std::cout << "mul  ";
            else if (type == 3) std::cout << "div ";
            else if (type == 4) std::cout << "mod ";

            if (left.is_constant) std::cout << left.value;
            else std::cout << "%" << left.value;
            std::cout << ", ";

            if (right.is_constant) std::cout << right.value;
            else std::cout << "%" << right.value;
            std::cout << std::endl;

            return ExprResult(false, koopacnt++);
            
            return ExprResult();
        }
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
// UnaryOp ::= "+" | "-" | "!";
class UnaryExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> primaryexp_unaryexp;
        char unaryop;
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
                if (unaryop == '+') return operand;
                std::cout <<  "  %" << koopacnt << " = ";
                if (unaryop == '-') std::cout << "sub 0, ";
                else if (unaryop == '!') std::cout << "eq 0, ";

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