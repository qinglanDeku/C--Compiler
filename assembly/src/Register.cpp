#include"Register.h"
#include"Assembly.h"
/*********************class Register***************************/

Register::Register():state(false), name(""), address(0), variableName(""), value(0), asmType(Register::NONE){}

Register::Register(string &name):state(false), name(name), address(0), variableName(""), value(0), asmType(Register::NONE) {}

Register::~Register(){}

void Register::setVar(Variable &var){
    state = true;
    address = var.getAddr();
    variableName = var.getName();
}


/**************************class MipsRegisterList*************************/



MipsRegisterList::MipsRegisterList(){
    for (int i(0); i < 32; i++){
        regs.push_back(Register(Mips32Regs[i]));
    }
    for (int i(8); i < 24; i++)
    {
        emptyRegList.push_back(&regs[i]);
    }
    //25号留出
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

Register* MipsRegisterList::getRegViaVarName(const string& VarName){
    for (int i(0); i < regs.size(); i++)
    {
        if(regs[i].getVarName() == VarName )
            return &regs[i];
    }
    return nullptr;
}

Register& MipsRegisterList::allocateReg(Assembly& assembly_process){
    if(emptyRegList.size() != 0){
        Register *p = emptyRegList.front();
        emptyRegList.pop_front();
        occupiedRegList.push_back(p);
        p->setState(true);
        return *p;
    }
    else{
        Register *p(occupiedRegList.front());
        occupiedRegList.pop_front();
        spillOneReg(*p, assembly_process);
        occupiedRegList.push_back(p);
        p->setState(true);
        return *p;
    }
}


void MipsRegisterList::spillOneReg(Register &r, Assembly &asm_process){
     //只有寄存器里存放的值是非数组变量才有溢出的意义
        AsmCode *newCode = new AsmCode("  sw ");
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, getRegNumber(r), -1));
        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, r.getVarAddress(), 30));
        asm_process.addAsmCode(*newCode);
        asm_process.asmVarList.getVar(r.getVarName()).setReg(0);
        delete newCode;
        newCode = nullptr;
    r.setVarName("");
    //r.setAsmType(Register::NONE);
    r.setState(false);
    r.setAddress(0);
    for (deque<Register*>::iterator it = occupiedRegList.begin(); it != occupiedRegList.end(); it++){
        if((*it)->getRegName() == r.getRegName()){
            emptyRegList.push_back(*it);
            occupiedRegList.erase(it);
            break;
        }
    }
}

/*before retract must have spilled*/
void MipsRegisterList::retractReg(int number, Assembly &asm_process){
    for (deque<Register *>::iterator p(occupiedRegList.begin()); p != occupiedRegList.end(); p++){
        if((*p)->getRegName() == regs[number].getRegName()){
            spillOneReg(*(*p), asm_process);
            (*p)->setState(false);
            occupiedRegList.erase(p);
            break;
        }
    }
}

void MipsRegisterList::retractReg(const string &name, Assembly &asm_process){
    for (deque<Register *>::iterator p(occupiedRegList.begin()); p != occupiedRegList.end(); p++){
        if((*p)->getRegName() == name){
            spillOneReg(*(*p), asm_process);
            (*p)->setState(false);
            occupiedRegList.erase(p);
            break;
        }
    }
}

void MipsRegisterList::spillAllReg(Assembly &asm_process){
    for (int i(0); i < occupiedRegList.size(); i++){
        spillOneReg(*(occupiedRegList[i]), asm_process);
    }
    occupiedRegList.clear();
    emptyRegList.clear();
    for (int i(8); i< 24; i++){
        emptyRegList.push_back(&regs[i]);
    }
}

void MipsRegisterList::clearReg(Assembly &asm_process){
    for (int i(0); i < occupiedRegList.size(); i++){
        asm_process.asmVarList.getVar(occupiedRegList[i]->getVarName()).setReg(0);
        occupiedRegList[i]->setVarName("");
        //r.setAsmType(Register::NONE);
        occupiedRegList[i]->setState(false);
        occupiedRegList[i]->setAddress(0);
    }
    occupiedRegList.clear();
    emptyRegList.clear();
    for (int i(8); i< 24; i++){
        emptyRegList.push_back(&regs[i]);
    }
}
