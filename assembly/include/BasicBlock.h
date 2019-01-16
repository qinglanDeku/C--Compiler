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
  BasicBlock(int begib ,int end, list<InterCode *>::iterator BP, list<InterCode *>::iterator EP);
  BasicBlock(const BasicBlock &a);
  ~BasicBlock();
  list<InterCode *>::iterator getCode(int number)const ;
  string getOneLine(int i);                 //获得某行中间代码
  int getBegin() const{ return begin; }          //获得中间代码的起始行号；
  int getEnd()const { return end; }              //获得中间代码的结束行号；
  int getSize() const{ return end - begin + 1; } //获得基本块中间代码的行数；
  InterCode::IRtype getCodeType(int i);     //获得中间代码的type
  InterCode::IRtype getCodeType(string code);


private:
  int begin, end;
  list<InterCode*>::iterator BP, EP;
};


class BlockList{
public:
  BlockList();
  BlockList(list<InterCode *> &codeList);
  ~BlockList();
  int size() { return blockList.size(); }
  const BasicBlock& getBlock(int number) { return blockList[number]; }

private:
  vector<BasicBlock> blockList;
};