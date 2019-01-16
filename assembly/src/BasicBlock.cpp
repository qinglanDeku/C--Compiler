#include"BasicBlock.h"
#include<iostream>
//#define DEBUG 1
/***********class BasicBLock****************/
BasicBlock::BasicBlock() : begin(0), end(0){};

BasicBlock::BasicBlock(int begin , int end, list<InterCode *>::iterator BP, list<InterCode *>::iterator EP) : \
begin(begin), end(end), BP(BP), EP(EP){};

BasicBlock::BasicBlock(const BasicBlock &a) : begin(a.begin), end(a.end), BP(a.BP), EP(a.EP){}

BasicBlock::~BasicBlock(){}

list<InterCode *>::iterator BasicBlock::getCode(int number)const {
    list<InterCode *>::iterator p(BP);
    for (int i(0); i < number; i++)
    {
        p++;
    }
    return p;
}

string BasicBlock::getOneLine(int number){
    return (*getCode(number))->getCode();
}

InterCode::IRtype BasicBlock::getCodeType(int number){
    return (*getCode(number))->getType();
}

InterCode::IRtype BasicBlock::getCodeType(string code){
    list<InterCode *>::iterator p(BP);
    while(p != EP){
        if((*p)->getCode() == code){
            return (*p)->getType();
        }
        p++;
    }
}


/*******************class BlockList**************************************/
BlockList::BlockList(){};

BlockList::BlockList(list<InterCode *> &codelist){
#ifdef DEBUG
    std::cout << "begin divide Block" << std::endl;
#endif
    list<InterCode *>::iterator BP(codelist.begin()), p(codelist.begin());
    int begin(0), end(0);
    while(p != codelist.end()){
        p++;
        end++;
        while (p != codelist.end() && (*p)->getType() != InterCode::FUNC)
        {
            p++;
            end++;
        }
        if(p == codelist.end()){
            blockList.push_back(BasicBlock(begin, end - 1 , BP, p));
        }
        else{
            p--;
            end--;
            blockList.push_back(BasicBlock(begin, end, BP, p));
            p++;
            end++;
            BP = p;
            begin = end;
        }
    }

#ifdef DEBUG
    for (int i(0); i < blockList.size(); i++){
        std::cout << blockList[i].getBegin() << "   " << blockList[i].getEnd() << std::endl;
    }
#endif
}

BlockList::~BlockList(){}

