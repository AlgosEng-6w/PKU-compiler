#pragma once
#include<memory>
#include<string>
#include<iostream>
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
            std::cout << std::endl << " }" << std::endl;   
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
            std::cout << "  ";
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
            std::cout << "ret ";
            exp->KoopaIR();
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
class PrimaryExpAST : public BaseAST{
    public:
        int type;
        std::unique_ptr<BaseAST> exp_number;

        void Dump() const override{
            std::cout << "PrimaryExpAST { ";
            exp_number->Dump();
            std::cout << " }";
        }

        void KoopaIR() const override;
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST{
    public:
        std::unique_ptr<BaseAST> primaryexp_unaryop;
        std::unique_ptr<BaseAST> unaryexp;
        int type;
        
        void Dump() const override{
            std::cout << "UnaryExpAST { ";
            primaryexp_unaryop->Dump();
            if (type == 2) unaryexp->Dump();
            std:cout << " }";
        }

        void KoopaIR() const override;
};

// UnaryOp ::= "+" | "-" | "!";
class UnaryOpAST : public BaseAST{
    public:
        char op;
        
        void Dump() const override{
            std::cout << "UnaryOpAST { " << op << " }";
        }

        void KoopaIR() const override;
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
    public:
        std::int32_t int_const;

        void Dump() const override{
            std::cout << "Number { " << int_const << " }";
        }

        void KoopaIR() const override{
            std::cout << int_const;
        }
};