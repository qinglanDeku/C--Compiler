#include"Assembly.h"
#include"../../semantic/include/Symboltab.h"
#include<iostream>
#include<fstream>
#include<sstream>
using std::cout;
using std::endl;
using std::ofstream;
using std::stringstream;
/*****************class Variable*******************/
Variable::Variable():origin(nullptr), addr(0),reg(0){}

Variable::Variable(int addr, int reg, VariableOP *var):origin(var), addr(addr),reg(reg){}

Variable::Variable(const Variable& a):origin(a.origin), addr(a.addr),reg(a.reg){}

Variable::~Variable() { origin = nullptr; }


/*******************class VariableList**********************/

VariableList::VariableList(){}

VariableList::~VariableList(){}

Variable& VariableList::getVar(const string &name){
    for (int i(0); i < VarList.size(); i++){
        if(VarList[i].getName() == name)
            return VarList[i];
    }
}



/**********************class AsmOperand************************/

AsmOperand::AsmOperand(AsmOPType type, int part1, int part2):type(type), part1(part1), part2(part2){
    AsmOPStr = produceStrOperand();
}

AsmOperand::AsmOperand(const AsmOperand& a):type(a.type), part1(a.part1), part2(a.part2),\
AsmOPStr(AsmOPStr){}

AsmOperand::~AsmOperand(){}

string AsmOperand::produceStrOperand(){
    string retval;
    if (type == IMMEDIATE)
    {
        stringstream ss;
        ss << part1;
        ss >> retval;
        return retval;
    }
    else if(type == REGISTER)
    {
        retval.append("$");
        retval.append(MipsRegisterList::Mips32Regs[part1]);
        return retval;
    }
    else if(type == LABEL){
        retval.append("label");
        stringstream ss;
        ss << part1;
        string temp;
        ss >> temp;
        retval.append(temp);
        retval.append(":");
        return retval;
    }
    else{
        stringstream ss;
        ss << part1;
        ss >> retval;
        retval.append("($");
        retval.append(MipsRegisterList::Mips32Regs[part2]);
        retval.append(")");
        return retval;
    }
}


/*************************class AsmCode***************************/
AsmCode::AsmCode(string AsmOperator):AsmOperator(AsmOperator),strCode(""){}

AsmCode::AsmCode(const AsmCode& a):AsmOperator(a.AsmOperator), OperandList(a.OperandList),\
strCode(a.strCode){}

AsmCode::~AsmCode(){}

string AsmCode::produceStrCode(){
    string retval;
    retval.append(AsmOperator);
    for (int i(0); i < OperandList.size() - 1; i++){
        retval.append(OperandList[i].getStrOperand());
        retval.append(", ");
    }
    retval.append(OperandList[OperandList.size() - 1].getStrOperand());

}

/*******************class Assembly****************************************/

Assembly::Assembly(){}

Assembly::Assembly(list<InterCode *> IRCodeList, list<VariableOP *> variableList):\
IRCodeList(IRCodeList), variableList(variableList), blockList(IRCodeList), Mips32(){}

Assembly::~Assembly(){}

void Assembly::printAssembly(){
    for (int i(0); i < AssemblyCodeList.size(); i++){
        cout << AssemblyCodeList[i].getStrAsmCode() << endl;
    }
}

void Assembly::outputAssembly(const string& filename){
    string outputFileName("");
    for (int i(0); i < filename.size(); i++)
    {
        if(filename[i] == '.'){
            string temp(filename, 0, i + 1);
            outputFileName = temp;
            outputFileName.append("S");
        }
    }
    if(outputFileName == "")
    {
        outputFileName = filename;
        outputFileName.append(".S");
    }
    ofstream out(filename, std::ios::out);
    if(!out){
        std::cerr << "error when output \".S\" file" << endl;
        exit(-1);
    }
    for (int i(0); i < AssemblyCodeList.size(); i++){
        out << AssemblyCodeList[i].getStrAsmCode() << endl;
    }
    out.close();
}


void Assembly::produceAssembly(){
    for (int i(0); i < blockList.size(); i++){
        translateOneBlock(blockList.getBlock(i));
    }
}

void Assembly::translateOneBlock(const BasicBlock &BB){
    int FPoffset(0);    //每一个代码块即函数块中分配给变量的相对于帧指针的偏移量。
    for (int i(0); i < BB.getSize();)
    {
        list<InterCode *>::iterator codeP(BB.getCode(i));
        int moveOn(translateOneLine(codeP, FPoffset));
        i = i + moveOn;
    }
}

int Assembly::translateOneLine(list<InterCode *>::iterator it, int &offset){
    int retval(1);
    if ((*it)->getType() == InterCode::FUNC)/*对于函数声明的中间代码*/
    {   
        /*标注函数名字并保存帧指针*/
        InterCode *ICP((*it));
        FuncDecCode *FuncP = (FuncDecCode *)ICP;
        string FuncName = FuncP->getFuncName();
        FuncName.append(":");
        AsmCode *newCode = new AsmCode(FuncName);
        AssemblyCodeList.push_back(*newCode);
        delete newCode;
        newCode = nullptr;
        AsmCode *newCode = new AsmCode("addi ");
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        newCode->addOperand(AsmOperand(AsmOperand::IMMEDIATE, -4, -1));
        AssemblyCodeList.push_back(*newCode);
        delete newCode;
        newCode = new AsmCode("sw ");
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 30, -1));
        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
        AssemblyCodeList.push_back(*newCode);
        delete newCode;
        newCode = new AsmCode("move ");
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 30, -1));
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        AssemblyCodeList.push_back(*newCode);
        delete newCode;
        newCode = nullptr;
        /*上面部分是每次进入函数体后必须要进行的部分：对栈帧指针作调整*/
        list<InterCode *>::iterator p(++it);
        int num_of_param(0);
        while((*p)->getType() == InterCode::PARAM){
            num_of_param++;
            p++;
        }
        retval += num_of_param;
        /*上部分在统计参数的数量, 下面给出参数的存储位置*/
        if(num_of_param <= 4){/*参数数量少于等于4，所以全部放在寄存器中*/
            p = it;
            /*把参数依次放入寄存器中*/
            while(num_of_param > 0){
                Operand *temp = (*p)->getOperand(1);
                VariableOP *varP = (VariableOP *)temp;
                Variable *asmVarP = new Variable(0, 3 + num_of_param, varP);
                asmVarList.addVar(*asmVarP);
                delete asmVarP;
                asmVarP = nullptr;
                num_of_param -= 1;
                p++;
            }
        }
        else{
            p = it;
            int offset_of_param(8);
            while (num_of_param > 4)
            {
                Operand *temp = (*p)->getOperand(1);
                VariableOP *varP = (VariableOP *)temp;
                if(varP->getSymbolTabItem()->GetType() == INTARRAY)/*数组类型传递的参数是地址*/
                    offset_of_param += 4;
                else
                    offset_of_param += varP->getSymbolTabItem()->getSize();
                Variable *asmVarP = new Variable(offset_of_param, 0, varP);
                asmVarList.addVar(*asmVarP);
                delete asmVarP;
                asmVarP = nullptr;
                num_of_param -= 1;
                p++;
            }
            while(num_of_param > 0){
                Operand *temp = (*p)->getOperand(1);
                VariableOP *varP = (VariableOP *)temp;
                Variable *asmVarP = new Variable(0, 3 + num_of_param, varP);
                asmVarList.addVar(*asmVarP);
                delete asmVarP;
                asmVarP = nullptr;
                num_of_param -= 1;
                p++;
            }
        }
        /*给变量分配空间的工作可以留在翻译中间代码的部分，遇到一个就开辟一个*/

    }

    else if((*it)->getType() == InterCode::ARG){
        while((*it)->getType() == InterCode::ARG){
            retval += 1;
            it++;
        }
        /*栈帧的变化交给call来做*/
    }
    else if((*it)->getType() == InterCode::CALL){
        /*保存a0-a3*/
        AsmCode *newCode = new AsmCode("addi ");
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        newCode->addOperand(AsmOperand(AsmOperand::IMMEDIATE, -16, -1));
        AssemblyCodeList.push_back(*newCode);
        delete newCode;
        for (int i(0); i < 4;i++){
            newCode = new AsmCode("sw ");
            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 4 + i,-1));
            newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, -4 * i, 29));
            AssemblyCodeList.push_back(*newCode);
            delete newCode;
        }
        newCode = nullptr;
        /*传递参数（如果有), 统计ARG*/
        list<InterCode *>::iterator p(it);
        p--;
        int num_of_arg(0);
        while ((*p)->getType() == InterCode::ARG){
            num_of_arg += 1;
            if(num_of_arg <= 4){
                if((*p)->getOperand(1)->getType()==Operand::ICONSTANT){
                    //如果传递的参数是常数，那么直接将其写入寄存器。
                    Register* reg_being_used(Mips32.getReg(3 + num_of_arg));
                    reg_being_used->setState(true);
                    //Operand* OP_ptr = (*p)->getOperand(1);
                    ConstantOP *const_OP_ptr = getConstOPptr((*p)->getOperand(1));
                    reg_being_used->setValue(const_OP_ptr->getIntVal());
                }
                else{
                    //Operand *OP_ptr = (*p)->getOperand(1);
                    VariableOP *var_OP_ptr = getVariaOPptr((*p)->getOperand(1));
                    if (asmVarList.ifVarInList(var_OP_ptr->getName()))/*已经为变量开辟空间*/
                    {   
                        if(asmVarList.getVar(var_OP_ptr->getName()).getReg() == 0){
                            /*如果变量已经被溢出*/
                            newCode = new AsmCode("lw ");

                        }
                    }
                }
            }
            p--;
        }

        spillReg();//寄存器溢出
    }

    return retval;
}

int Assembly::translateParam(list<InterCode *>::iterator it, int offset){
    /*int retval(1);
    if ((*it)->getType() != InterCode::PARAM)
    {
        std::cerr << "wrong when translate Func and PARAM" << endl;
        exit(-1);
    }
    Operand *temp = (*it)->getOperand(1);
    VariableOP *varP = (VariableOP *)temp;
    int varPoffset(varP->getSymbolTabItem()->getSize());
    offset += varPoffset;
    Variable *asmVarP = new Variable(offset, varP);
    asmVarList.addVar(*asmVarP);
    delete asmVarP;
    it++;
    if((*it)->getType() == InterCode::PARAM){
        retval += translateParam(it, offset);
    }
    return retval;*/
    return 1;
}