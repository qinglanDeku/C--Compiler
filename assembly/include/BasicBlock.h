#pragma once
#include"../../IR/include/InterCode.h"
#include<string>
#include<list>
#include<vector>
using std::list;
using std::string;
using std::vector;

class BasicBlock{
public:
  BasicBlock();
  BasicBlock(list<InterCode *>::iterator BP, list<InterCode *>::iterator EP);
  ~BasicBlock();
  string getOneLine(int i);                 //获得某行中间代码
  int getBegin() { return begin; }          //获得中间代码的起始行号；
  int getEnd() { return end; }              //获得中间代码的结束行号；
  int getSize() { return end - begin + 1; } //获得基本块中间代码的行数；
  InterCode::IRtype getCodeType(int i);     //获得中间代码的type
  InterCode::IRtype getCodeType(string code);
  string getOperand(int i); //获得代码中的第i个操作数，如果没有返回空字符串。
  string getOperator();     //获得中间代码的操作符， 如果没有返回空字符串。

private:
  int begin, end;
  list<InterCode*>::iterator BP, EP;
};


class BlockList{
public:
  BlockList();
  BlockList(list<InterCode *> &codeList);
  ~BlockList();
  const BasicBlock& getBlock(int number) { return blockList[number]; }

private:
  vector<BasicBlock> blockList;
};