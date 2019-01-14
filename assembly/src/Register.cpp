#include"Register.h"
#include"Assembly.h"
/*********************class Register***************************/

Register::Register():state(false), name(""), address(0), variableName(""), value(0){}

Register::Register(string &name):state(false), name(name), address(0), variableName(""), value(0){}

Register::~Register(){}

void Register::setVar(Variable &var){
    state = true;
    address = var.getAddr();
    variableName = var.getName();
}


/**************************class MipsRegisterList*************************/

string MipsRegisterList::Mips32Regs[32] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

MipsRegisterList::MipsRegisterList(){
    for (int i(0); i < 32; i++){
        regs.push_back(Register(Mips32Regs[i]));
    }
    for (int i(8); i < 25; i++)
    {
        emptyRegList.push_back(&regs[i]);
    }
}

MipsRegisterList::~MipsRegisterList(){}

Register* MipsRegisterList::getReg(int number){
    return &regs[number];
}

Register* MipsRegisterList::getReg(const string& name){
    for (int i(0); i < regs.size(); i++)
    {
        if(regs[i].getRegName() == name )
            return &regs[i];
    }
    return nullptr;
}

/*leaving the spilling to translator*/
Register& MipsRegisterList::allocateReg(){
    if(emptyRegList.size() != 0){
        Register *p = emptyRegList.back();
        emptyRegList.pop_back();
        occupiedRegList.push_back(p);
        return *p;
    }
    else{
        Register *p(occupiedRegList.front());
        occupiedRegList.pop_front();
        /*????*/
        occupiedRegList.push_back(p);
        return *p;
    }
}

/*before retract must have spilled*/
void MipsRegisterList::retractReg(int number){
    for (deque<Register *>::iterator p(occupiedRegList.begin()); p != occupiedRegList.end(); p++){
        if((*p)->getRegName() == regs[number].getRegName()){
            occupiedRegList.erase(p);
            break;
        }
    }
}

void MipsRegisterList::retractReg(const string &name){
    for (deque<Register *>::iterator p(occupiedRegList.begin()); p != occupiedRegList.end(); p++){
        if((*p)->getRegName() == name){
            occupiedRegList.erase(p);
            break;
        }
    }
}


