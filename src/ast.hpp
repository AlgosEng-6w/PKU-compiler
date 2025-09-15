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

class FuncTypeAST : public BaseAST{
    public:
        void Dump() const override{
            std::cout << "FuncTypeAST { int }";
        }
        
        void KoopaIR() const override{
            std::cout << "i32" << std::endl;
        };
};

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

class StmtAST : public BaseAST{
    public:
        std::int32_t number;

        void Dump() const override{
            std::cout << "StmtAST { " << number << " }";
        }

        void KoopaIR() const override{
            std::cout << "ret " << number;
        }
};
