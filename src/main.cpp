#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include "ast.hpp"

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

  }
  cout.rdbuf(oldcoutbuf);
  outputfile.close();
  return 0;
}