#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include "ast.hpp"
#include "koopa.h"
#include "visitraw.hpp"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  yyin = fopen(input, "r");
  assert(yyin);

  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  cout << "=== AST Structure ===" << endl;
  ast->Dump();
  cout << endl;

  ofstream outputfile(output);
  assert(outputfile);

  stringstream ss;
  streambuf *oldcoutbuf = cout.rdbuf(ss.rdbuf());
  ast->KoopaIR();
  cout.rdbuf(outputfile.rdbuf());

  if (string(mode)=="-koopa"){
    cout << ss.str();
  }
  else if (string(mode)=="-riscv")
  {
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(ss.str().c_str(),&program);
    assert(ret == KOOPA_EC_SUCCESS);
    
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    koopa_delete_program(program);

    // for (size_t i = 0; i < raw.funcs.len; ++i) {
    //   assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
    //   koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];

    //   for (size_t j = 0; j < func->bbs.len; ++j){
    //     assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
    //     koopa_raw_basic_block_t bb= (koopa_raw_basic_block_t) func->bbs.buffer[j];

    //     for (size_t k = 0; k < bb->insts.len; ++k){
    //       assert(bb->insts.kind == KOOPA_RSIK_VALUE);
    //       koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[k];

    //       switch (value->kind.tag){

    //       }
    //     }
    //   }
    // }

    Visit(raw);

    koopa_delete_raw_program_builder(builder);
  }
  cout.rdbuf(oldcoutbuf);
  outputfile.close();
  return 0;
}