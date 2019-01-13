#pragma once
#include<vector>
#include<list>
#include<deque>
#include<string>
#include"../../IR/include/InterCode.h"

class Variable{
public:
  Variable();
  Variable(unsigned int addr, VariableOP*var);
  ~Variable();
  unsigned int getAddr() { return addr; }
  string getName() { return origin->getName(); }
  Operand::OPType getVarType() { return origin->getType(); }
  int getVarSize() { return origin->getSymbolTabItem()->getSize(); }

private:
    VariableOP *origin;
    unsigned int addr; //the offset from bp;
};
