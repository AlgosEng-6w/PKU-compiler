#pragma once
#include<cassert>
#include<iostream>
#include "koopa.h"
#include<map>
#include<string>

std::map<koopa_raw_value_t, std::string> regs;
int reg_count = 0;
std::string new_reg() {
    return "t" + std::to_string(reg_count++);
}

void Visit(const koopa_raw_slice_t &slice);   
void Visit(const koopa_raw_function_t &func);      
void Visit(const koopa_raw_basic_block_t &bb);     
void Visit(const koopa_raw_value_t &value);        
void Visit(const koopa_raw_return_t &value);       
void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary);
void Visit(const koopa_raw_integer_t &value);    

void Visit(const koopa_raw_program_t &program){
    Visit(program.values);
    Visit(program.funcs);
}

void Visit(const koopa_raw_slice_t &slice){
    for (size_t i = 0; i < slice.len; ++i){
        auto ptr = slice.buffer[i];
        switch (slice.kind){
            case KOOPA_RSIK_FUNCTION:
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            case KOOPA_RSIK_VALUE:
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            default:
                assert(false);
        }
    }
}

void Visit(const koopa_raw_function_t &func){
    std::cout << " .text" << std::endl;
    std::cout << " .global " << func->name+1 << std::endl;
    std::cout << func->name+1 << ":" << std::endl;

    reg_count = 0;
    regs.clear();
    Visit(func->bbs);
}

void Visit(const koopa_raw_basic_block_t &bb){

    Visit(bb->insts);
}

void Visit(const koopa_raw_value_t &value){
    if (regs.count(value)) {
        return;
    }

    const auto &kind = value->kind;
    switch(kind.tag) {
        case KOOPA_RVT_RETURN:
            Visit(kind.data.ret);
            break;
        case KOOPA_RVT_INTEGER:
            Visit(kind.data.integer);
            break;
        case KOOPA_RVT_BINARY:
            Visit(value, kind.data.binary);
            break;
        default:
            assert(false);
    }
}

void Visit(const koopa_raw_return_t &value){
    koopa_raw_value_t ret_val = value.value;
    if (ret_val->kind.tag == KOOPA_RVT_INTEGER) {
        std::cout << "  li a0, " << ret_val->kind.data.integer.value << std::endl;
    } else {
        Visit(ret_val);
        std::cout << "  mv a0, " << regs.at(ret_val) << std::endl;
    }

    std::cout << "  ret" << std::endl;
}

void Visit(const koopa_raw_integer_t &value){

}

void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary) {
    koopa_raw_value_t lhs_val = binary.lhs;
    koopa_raw_value_t rhs_val = binary.rhs;

    std::string lhs_reg;
    if (lhs_val->kind.tag == KOOPA_RVT_INTEGER) {
        int32_t lhs_value = lhs_val->kind.data.integer.value;
        if (lhs_value == 0) lhs_reg = "x0";
        else {
            lhs_reg = new_reg();
            std::cout << "  li " << lhs_reg << ", " << lhs_value << std::endl;
        }
    }
    else {
        Visit(lhs_val);
        lhs_reg = regs.at(lhs_val);
    }

    std::string rhs_reg;
    if (rhs_val->kind.tag == KOOPA_RVT_INTEGER) {
        int32_t rhs_value = rhs_val->kind.data.integer.value;
        if (rhs_value == 0) rhs_reg = "x0";
        else {
            rhs_reg = new_reg();
            std::cout << "  li " << rhs_reg << ", " << rhs_value <<std::endl;
        }
    }
    else {
        Visit(rhs_val);
        rhs_reg = regs.at(rhs_val);
    }

    std::string result_reg = new_reg();
    regs[value] = result_reg;

    switch (binary.op) {
        case KOOPA_RBO_ADD:
            std::cout << "  add " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_SUB:
            std::cout << "  sub " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_MUL:
            std::cout << "  mul " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_DIV:
            std::cout << "  div " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_MOD:
            std::cout << "  rem " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_EQ:   // 相等比较
            std::cout << "  xor " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            std::cout << "  seqz " << result_reg << ", " << result_reg << std::endl;
            break;
        case KOOPA_RBO_NOT_EQ:  // 不等比较
            std::cout << "  xor " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            std::cout << "  snez " << result_reg << ", " << result_reg << std::endl;
            break;
        case KOOPA_RBO_LT:   // 小于
            std::cout << "  slt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            break;
        case KOOPA_RBO_LE:   // 小于等于
            // a <= b 等价于 !(a > b) 等价于 !(b < a)
            std::cout << "  slt " << result_reg << ", " << rhs_reg << ", " << lhs_reg << std::endl;
            std::cout << "  seqz " << result_reg << ", " << result_reg << std::endl;
            break;
        case KOOPA_RBO_GT:   // 大于
            std::cout << "  slt " << result_reg << ", " << rhs_reg << ", " << lhs_reg << std::endl;
            break;
        case KOOPA_RBO_GE:   // 大于等于
            // a >= b 等价于 !(a < b)
            std::cout << "  slt " << result_reg << ", " << lhs_reg << ", " << rhs_reg << std::endl;
            std::cout << "  seqz " << result_reg << ", " << result_reg << std::endl;
            break;
        case KOOPA_RBO_OR:
            std::cout << "  or t0, " << lhs_reg << ", " << rhs_reg << std::endl;
            std::cout << "  snez " << result_reg << ", t0" << std::endl;
            break;
        case KOOPA_RBO_AND:
            std::cout << "  snez t0, " << lhs_reg << std::endl;
            std::cout << "  snez t1, " << rhs_reg << std::endl;
            std::cout << "  and " << result_reg << ", t0, t1" << std::endl;
            break;
        default:
            assert(false);
    }
}