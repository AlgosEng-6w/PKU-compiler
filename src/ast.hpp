#pragma once
#include<memory>
#include<string>
#include<iostream>

static int koopacnt;   // 临时变量计数器

class BaseAST {
    public:
        virtual ~BaseAST() = default;
        virtual void Dump() const = 0;
        virtual void KoopaIR() const = 0;
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

        void KoopaIR() const override {
            func_def->KoopaIR();
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

        void KoopaIR() const override {
            std::cout << "fun @" << ident << "(): ";
            func_type->KoopaIR();
            std::cout << "{ " << std::endl;
            block->KoopaIR();
            std::cout << " }" << std::endl;   
        }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST{
    public:
        void Dump() const override{
            std::cout << "FuncTypeAST { int }";
        }
        
        void KoopaIR() const override{
            std::cout << "i32" << std::endl;
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

        void KoopaIR() const override{
            std::cout << "%entry:" << std::endl;
            stmt->KoopaIR();
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

        void KoopaIR() const override{
            exp->KoopaIR();
            std::cout << "  ret %" << koopacnt-1 << std::endl;
        }
};

// Exp ::= UnaryExp;
class ExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> unaryexp;

        void Dump() const override{
            std::cout << "ExpAST { ";
            unaryexp->Dump();
            std::cout << " }";
        }

        void KoopaIR() const override{
            unaryexp->KoopaIR();
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

        void KoopaIR() const override{
            if (type == 1) exp->KoopaIR();
            else if (type == 2){
                std::cout << number << std::endl;
            }
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

        void KoopaIR() const override{
            int constvalue;
            if (isConstantExpression(constvalue) && unaryop == '!') {
                std::cout <<  "  %" << koopacnt << " = eq 0, " << constvalue << std::endl;
                koopacnt++;
                return ;
            }
            primaryexp_unaryexp->KoopaIR();
            if (type == 2){
                if (unaryop == '-') {
                    std::cout <<  "  %" << koopacnt << " = sub 0, %" << koopacnt-1 << std::endl;
                    koopacnt++;
                }
                else if (unaryop == '!') {
                    
                    std::cout <<  "  %" << koopacnt << " = eq 0, %" << koopacnt-1 << std::endl;
                    koopacnt++;
                }
                else if (unaryop == '+'){
                    
                }
            }
        }

    private:
        bool isConstantExpression(int& outValue) const {
            PrimaryExpAST* primary = dynamic_cast<PrimaryExpAST*>(primaryexp_unaryexp.get());
            if (primary && primary->type == 2) {
                outValue = primary->number;
                return true;
            }
            return false;
        }
};


