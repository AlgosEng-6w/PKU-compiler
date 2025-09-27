#pragma once
#include<cassert>
#include<iostream>
#include "koopa.h"
#include<unordered_map>
#include<string>

static int stack_frame_length = 0;
static int stack_frame_used = 0;

static std::unordered_map<koopa_raw_value_t, std::string> loc;

void Visit(const koopa_raw_slice_t &slice);   
void Visit(const koopa_raw_function_t &func);      
void Visit(const koopa_raw_basic_block_t &bb);     
void Visit(const koopa_raw_value_t &value);        
void Visit(const koopa_raw_return_t &ret);       
void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary);
void Visit(const koopa_raw_integer_t &integer);    
void Visit(const koopa_raw_load_t &load, const koopa_raw_value_t &value);
void Visit(const koopa_raw_store_t &store);

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

    stack_frame_length = 0;
    stack_frame_used = 0;

    int var_cnt = 0;
    for (size_t i = 0; i < func->bbs.len; ++i) {
        const auto& insts = reinterpret_cast<koopa_raw_basic_block_t>(func->bbs.buffer[i])->insts;
        var_cnt += insts.len;
        for (size_t j = 0; j < insts.len; ++j) {
            auto inst = reinterpret_cast<koopa_raw_value_t>(insts.buffer[j]);
            if (inst->ty->tag == KOOPA_RTT_UNIT) var_cnt--;
        }
    }

    stack_frame_length = var_cnt << 2;
    stack_frame_length = (stack_frame_length + 16 -1) & (~(16-1));

    if (stack_frame_length != 0) {
        std::cout << "  addi sp, sp, -" << stack_frame_length << std::endl;
    }

    Visit(func->bbs);
}

void Visit(const koopa_raw_basic_block_t &bb){

    Visit(bb->insts);
}

void Visit(const koopa_raw_value_t &value){
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
        case KOOPA_RVT_ALLOC:
            loc[value] = std::to_string(stack_frame_used) + "(sp)";
            stack_frame_used += 4;
            break;
        case KOOPA_RVT_LOAD:
            Visit(kind.data.load, value);
            break;
        case KOOPA_RVT_STORE:
            Visit(kind.data.store);
            break;
        
        default:
            assert(false);
    }
}

static void load2reg(const koopa_raw_value_t &value, const std::string &reg) {
    if (value->kind.tag == KOOPA_RVT_INTEGER) {
        std::cout << "  li " << reg << ", " << value->kind.data.integer.value << std::endl;
    }
    else {
        std::cout << "  lw " << reg << ", " << loc[value] <<  std::endl;
    }
}

void Visit(const koopa_raw_return_t &ret){
    load2reg(ret.value, "a0");
    if (stack_frame_length != 0) {
        std::cout << "  addi sp, sp, " << stack_frame_length << std::endl;
    }
    std::cout << "  ret" << std::endl;
}

void Visit(const koopa_raw_integer_t &integer){
    std::cout << "  li a0, " << integer.value << std::endl;
}

void Visit(const koopa_raw_value_t &value, const koopa_raw_binary_t &binary) {
    load2reg(binary.lhs, "t0");
    load2reg(binary.rhs, "t1");

    switch (binary.op) {
        case KOOPA_RBO_NOT_EQ:
            std::cout << "  xor t0, t0, t1" << std::endl;
            std::cout << "  snez t0, t0" << std::endl;
            break;
        case KOOPA_RBO_EQ:
            std::cout << "  xor t0, t0, t1" << std::endl;
            std::cout << "  seqz t0, t0" << std::endl;
            break;
        case KOOPA_RBO_GT:
            std::cout << "  sgt t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_LT:
            std::cout << "  slt t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_GE:
            std::cout << "  slt t0, t0, t1" << std::endl;
            std::cout << "  xori t0, t0, 1" << std::endl;
            break;
        case KOOPA_RBO_LE:
            std::cout << "  sgt t0, t0, t1" << std::endl;
            std::cout << "  xori t0, t0, 1" << std::endl;
            break;
        case KOOPA_RBO_ADD:
            std::cout << "  add t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_SUB:
            std::cout << "  sub t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_MUL:
            std::cout << "  mul t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_DIV:
            std::cout << "  div t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_MOD:
            std::cout << "  rem t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_AND:
            std::cout << "  and t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_OR:
            std::cout << "  or t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_XOR:
            std::cout << "  xor t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_SHL:
            std::cout << "  sll t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_SHR:
            std::cout << "  srl t0, t0, t1" << std::endl;
            break;
        case KOOPA_RBO_SAR:
            std::cout << "  sra t0, t0, t1" << std::endl;
            break;
    }

    loc[value] = std::to_string(stack_frame_used) + "(sp)";
    stack_frame_used += 4;
    std::cout << "  sw t0, " << loc[value] << std::endl;
}

void Visit(const koopa_raw_load_t &load, const koopa_raw_value_t &value){
    load2reg(load.src, "t0");
    loc[value] = std::to_string(stack_frame_used) + "(sp)";
    stack_frame_used += 4;
    std::cout << "  sw t0, " << loc[value] << std::endl;
}

void Visit(const koopa_raw_store_t &store) {
    load2reg(store.value, "t0");
    std::cout << "  sw t0, " << loc[store.dest] << std::endl;
}