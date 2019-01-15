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
Variable::Variable() : addr(0), reg(0), name(""), type(Operand::VARIABLE), size(0), baseSize(4){
}

Variable::Variable(int addr, int reg, Operand*var):addr(addr),reg(reg){
    name = var->getName();
    type = var->getType();
    if(type == Operand::VARIABLE || type == Operand::ARRAY_ADRESS || type == Operand::ARRAY_FIRST_ELEMENT){
        VariableOP *temp_ptr = (VariableOP *)var;
        size = temp_ptr->getSymbolTabItem()->getSize();
    }
    else{
        size = 4;
    }
    baseSize = 4;
}

Variable::Variable(const Variable& a):addr(a.addr),reg(a.reg),name(a.name), type(a.type), size(a.size), baseSize(a.baseSize){}

Variable::~Variable() {}


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

string Assembly::AsmHead[22] = {
    ".data", 
    "_prompt: .asciiz \"Enter an integer:\"", 
    "_ret: .asciiz \"\\n\"",
    ".globl main",
    ".text",
    "read:",
    "li $v0, 4",
    "la $a0, _prompt",
    "syscall",
    "li $v0, 5",
    "syscall",
    "jr $ra"
    "",
    "write:",
    "li $v0, 1",
    "syscall",
    "li $v0, 4",
    "la $a0, _ret",
    "syscall",
    "move $v0, $0",
    "jr $ra",
    ""
};

void Assembly::printAssembly(){
    for (int i(0); i < 22;i++){
        cout << AsmHead[i] << endl;
    }
    for (int i(0); i < AssemblyCodeList.size(); i++)
    {
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
    for (int i(0); i < 22; i++){
        out << AsmHead[i] << endl;
    }

    for (int i(0); i < AssemblyCodeList.size(); i++)
    {
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
            int offset_of_param(4);
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
        //给变量分配空间：
        int bp_offset(0);
        while (p != IRCodeList.end() && (*p)->getType() != InterCode::FUNC)
        {
            if((*p)->getType() == InterCode::ASSIGN){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::ASSIGNFROMLOC){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::ASSIGNLOC){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::PLUS){
                createSpace((*p)->getOperand(1), bp_offset);       
            }
            else if((*p)->getType() == InterCode::MUL){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::SUB){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::DIV){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::DEC){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            else if((*p)->getType() == InterCode::READ){
                createSpace((*p)->getOperand(1), bp_offset);
            }
            p++;
        }
        subSp(-bp_offset);
    }
    else if((*it)->getType() == InterCode::ARG){//遇到ARG表示要开始传递函数，但是也要单独考虑没有参数的函数
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

        /*统计ARG ， 传递参数*/
        list<InterCode *>::iterator p(it);
        int num_of_arg(0);
        while ((*p)->getType() != InterCode::CALL){ //遇到call表示传递参数完毕
            if((*p)->getType() == InterCode::ARG){  
                num_of_arg += 1;
                if(num_of_arg <= 4){
                    Register* reg_being_used(Mips32.getReg(3 + num_of_arg));
                    reg_being_used->setState(true);
                    if((*p)->getOperand(1)->getType()==Operand::ICONSTANT){
                        //如果传递的参数是常数，那么直接将其写入寄存器。

                        ConstantOP *const_OP_ptr = getConstOPptr((*p)->getOperand(1));
                        reg_being_used->setValue(const_OP_ptr->getIntVal());
                        newCode = new AsmCode("li ");
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                        newCode->addOperand(AsmOperand(AsmOperand::IMMEDIATE, const_OP_ptr->getIntVal(), -1));
                        addAsmCode(*newCode);
                        delete newCode;
                        newCode = nullptr;
                        //这里使用专用参数寄存器所以只考虑设置值
                    }
                    else{
                        Operand *OP_ptr = (*p)->getOperand(1);
                        if (!asmVarList.ifVarInList(OP_ptr->getName()))
                        {   //如果没有为变量开辟空间，
                            //也就是这个变量(非数组,数组变量一定有空间)没有初始化就被使用，那么就把它的值当成0处理。
                            newCode = new AsmCode("move ");
                            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 0, -1));
                            addAsmCode(*newCode);
                            delete newCode;
                            newCode = nullptr;
                        }
                        else{
                            //如果为变量开辟了空间，那么就检查是否有寄存器保存有该变量的值，没有的话就从内存搬运，有的话从寄存器直接拿出
                            if(asmVarList.getVar(OP_ptr->getName()).getReg() ==0 ){
                                Register &alloc_new_reg(Mips32.allocateReg(*this));
                                alloc_new_reg.setState(true);
                                alloc_new_reg.setVar(asmVarList.getVar(OP_ptr->getName()));
                                asmVarList.getVar(OP_ptr->getName()).setReg(Mips32.getRegNumber(alloc_new_reg));
                                moveFromMemtoReg(Mips32.getRegNumber(alloc_new_reg), asmVarList.getVar(OP_ptr->getName()).getAddr(), 30);
                                moveFromRegtoReg(3 + num_of_arg, Mips32.getRegNumber(alloc_new_reg));
                            }
                            else{
                                moveFromRegtoReg(3 + num_of_arg, asmVarList.getVar(OP_ptr->getName()).getReg());
                            }
                        }
                    }
                    //传递的参数不是常数,那么可能是临时变量(数组的元素！)
                    /* else if((*p)->getOperand(1)->getType() == Operand::TEMP_VARIABLE){
                        TemporaryOP *temp_OP_ptr = getTempOPptr((*p)->getOperand(1));
                        Register *reg_owe_temp = Mips32.getRegViaVarName(temp_OP_ptr->getName());
                        if(reg_owe_temp == nullptr){
                            std::cerr << "wrong with handle temporary" << endl;
                            exit(-1);
                        }
                        newCode = new AsmCode("move ");
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, Mips32.getRegNumber(*reg_owe_temp), -1));
                        addAsmCode(*newCode);
                        delete newCode;
                        newCode = nullptr;
                    }
                    else{//或者局部变量
                        VariableOP *var_OP_ptr = getVariaOPptr((*p)->getOperand(1));
                        if(!asmVarList.ifVarInList(var_OP_ptr->getName())){//如果没有为变量开辟空间，
                        //也就是这个变量(非数组,数组变量一定有空间)没有初始化就被使用，那么就把它的值当成0处理。
                            newCode = new AsmCode("move ");
                            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 0, -1));
                            addAsmCode(*newCode);
                            delete newCode;
                            newCode = nullptr;
                        }
                        else//已经为变量开辟空间
                        {   
                            if(asmVarList.getVar(var_OP_ptr->getName()).getReg() == 0){
                                //如果变量已经被溢出，或者不是保存在寄存器里的变量，能被溢出的一定是存放在栈中的数据，存放在a0-a3的数据已经被提前保存好了;
                                if(var_OP_ptr->getType() == Operand::VARIABLE){//非数组变量
                                    if(asmVarList.getVar(var_OP_ptr->getName()).getAddr() != 0){//传递的参数在栈中而不是寄存器(调用者参数作为参数传递)
                                        newCode = new AsmCode("lw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, asmVarList.getVar(var_OP_ptr->getName()).getAddr(), 30));
                                    }
                                    else{//传递的参数在寄存器中（把调用者的参数传入）,这一段多余？
                                        newCode = new AsmCode("move ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, asmVarList.getVar(var_OP_ptr->getName()).getReg(), -1));
                                    }
                                    addAsmCode(*newCode);
                                    delete newCode;
                                    newCode = nullptr;
                                }
                                else{//数组变量传递地址
                                    if(asmVarList.getVar(var_OP_ptr->getName()).getAddr() != 0){
                                        newCode = new AsmCode("lw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, asmVarList.getVar(var_OP_ptr->getName()).getAddr(), 30));
                                        addAsmCode(*newCode);
                                        delete newCode;
                                        newCode = nullptr;
                                    }
                                    else{
                                        ///不可能出现这个情况...存在寄存器里的值会在另外一个分支处理
                                    }
                                }
                            }
                            else{//如果变量没有溢出，或者本来就是存放在寄存器中变量
                                //Register *reg_owe_var = Mips32.getReg(asmVarList.getVar(var_OP_ptr->getName()).getReg());
                                newCode = new AsmCode("move ");
                                newCode->addOperand(AsmOperand(AsmOperand::REGISTER, num_of_arg + 3, -1));
                                newCode->addOperand(AsmOperand(AsmOperand::REGISTER, asmVarList.getVar(var_OP_ptr->getName()).getReg(), -1));
                                addAsmCode(*newCode);
                                delete newCode;
                                newCode = nullptr;
                            }
                        }
                    }*/
                }
                else{//参数数量多于四个
                    /*if((*p)->getOperand(1)->getType()==Operand::ICONSTANT){
                        //如果传递的参数是常数，那么直接将其写入栈中。
                        subSp(4);
                        Register &r = Mips32.allocateReg(*this);
                        ConstantOP *const_OP_ptr = getConstOPptr((*p)->getOperand(1));
                        //reg_being_used->setValue(const_OP_ptr->getIntVal());
                        newCode = new AsmCode("li ");
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                        newCode->addOperand(AsmOperand(AsmOperand::IMMEDIATE, const_OP_ptr->getIntVal(), -1));
                        addAsmCode(*newCode);
                        delete newCode;
                        newCode = new AsmCode("sw ");
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                        addAsmCode(*newCode);
                        delete newCode;
                        newCode = nullptr;
                        //这里使用专用参数寄存器所以只考虑设置值
                    }
                    //传递的参数不是常数,那么可能是临时变量(数组的元素！)
                    else if((*p)->getOperand(1)->getType() == Operand::TEMP_VARIABLE){
                        subSp(4);
                        TemporaryOP *temp_OP_ptr = getTempOPptr((*p)->getOperand(1));
                        Register *reg_owe_temp = Mips32.getRegViaVarName(temp_OP_ptr->getName());
                        if(reg_owe_temp == nullptr){
                            std::cerr << "wrong with handle temporary" << endl;
                            exit(-1);
                        }
                        newCode = new AsmCode("sw ");
                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, Mips32.getRegNumber(*reg_owe_temp), -1));
                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                        addAsmCode(*newCode);
                        delete newCode;
                        newCode = nullptr;
                    }
                    else{//或者局部变量
                        VariableOP *var_OP_ptr = getVariaOPptr((*p)->getOperand(1));
                        subSp(4);
                        if (!asmVarList.ifVarInList(var_OP_ptr->getName()))
                        {   //如果没有为变量开辟空间，
                            //也就是这个变量(非数组,数组变量一定有空间)没有初始化就被使用，那么就把它的值当成0处理。
                            newCode = new AsmCode("sw ");
                            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 0, -1));
                            newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                            addAsmCode(*newCode);
                            delete newCode;
                            newCode = nullptr;
                        }
                        else//已经为变量开辟空间
                        {   
                            if(asmVarList.getVar(var_OP_ptr->getName()).getReg() == 0){
                                /*如果变量已经被溢出，或者不是保存在寄存器里的变量，能被溢出的一定是存放在栈中的数据，存放在a0-a3的数据已经被提前保存好了
                                if(var_OP_ptr->getType() == Operand::VARIABLE){//非数组变量
                                    if(asmVarList.getVar(var_OP_ptr->getName()).getAddr() != 0){//传递的参数在栈中而不是寄存器(调用者参数作为参数传递)
                                        newCode = new AsmCode("lw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, asmVarList.getVar(var_OP_ptr->getName()).getAddr(), 30));
                                        addAsmCode(*newCode);
                                        delete newCode;
                                        newCode = new AsmCode("sw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                                        addAsmCode(*newCode);
                                        delete newCode;
                                        newCode = nullptr;
                                    }
                                    /*else{//传递的参数在寄存器中（把调用者的参数传入）,这一段多余？
                                        newCode = new AsmCode("move ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 3 + num_of_arg, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, asmVarList.getVar(var_OP_ptr->getName()).getReg(), -1));
                                    }
                                }
                                else{//数组变量传递地址
                                    if(asmVarList.getVar(var_OP_ptr->getName()).getAddr() != 0){
                                        newCode = new AsmCode("lw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, asmVarList.getVar(var_OP_ptr->getName()).getAddr(), 30));
                                        addAsmCode(*newCode);
                                        delete newCode;
                                        newCode = new AsmCode("sw ");
                                        newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 25, -1));
                                        newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                                        addAsmCode(*newCode);
                                        delete newCode;
                                        newCode = nullptr;
                                    }
                                    else{
                                        /*不可能出现这个情况...存在寄存器里的值会在另外一个分支处理
                                    }
                                }
                            }
                            else{//如果变量没有溢出，或者本来就是存放在寄存器中变量
                                //Register *reg_owe_var = Mips32.getReg(asmVarList.getVar(var_OP_ptr->getName()).getReg());
                                newCode = new AsmCode("sw ");
                                newCode->addOperand(AsmOperand(AsmOperand::REGISTER, asmVarList.getVar(var_OP_ptr->getName()).getReg(), -1));
                                newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
                                addAsmCode(*newCode);
                                delete newCode;
                                newCode = nullptr;
                            }
                        }
                    }*/
                    if((*p)->getOperand(1)->getType()==Operand::ICONSTANT){
                        //如果传递的参数是常数，那么直接将其写入。

                        ConstantOP *const_OP_ptr = getConstOPptr((*p)->getOperand(1));
                        loadConsttoReg(const_OP_ptr->getIntVal(), 25);
                        subSp(4);
                        moveFromRegtoMem(29, 0, 25);
                    }
                    else{
                        Operand *OP_ptr = (*p)->getOperand(1);
                        if (!asmVarList.ifVarInList(OP_ptr->getName()))
                        {   //如果没有为变量开辟空间，
                            //也就是这个变量(非数组,数组变量一定有空间)没有初始化就被使用，那么就把它的值当成0处理。
                            subSp(4);
                            moveFromRegtoMem(29, 0, 0);
                        }
                        else{
                            //如果为变量开辟了空间，那么就检查是否有寄存器保存有该变量的值，没有的话就从内存搬运，有的话从寄存器直接拿出
                            if(asmVarList.getVar(OP_ptr->getName()).getReg() ==0 ){
                                subSp(4);
                                Register &alloc_new_reg(Mips32.allocateReg(*this));
                                alloc_new_reg.setState(true);
                                alloc_new_reg.setVar(asmVarList.getVar(OP_ptr->getName()));
                                asmVarList.getVar(OP_ptr->getName()).setReg(Mips32.getRegNumber(alloc_new_reg));
                                moveFromMemtoReg(Mips32.getRegNumber(alloc_new_reg), asmVarList.getVar(OP_ptr->getName()).getAddr(), 30);
                                moveFromRegtoMem(29, 0, Mips32.getRegNumber(alloc_new_reg));
                            }
                            else{
                                subSp(4);
                                moveFromRegtoMem(29, 0,  asmVarList.getVar(OP_ptr->getName()).getReg());
                            }
                        }
                    }
                }
            }
            else{
                std::cerr << "error : some sentences are between ARG" << endl;
                exit(-1);
            }
            p++;
            retval++;
        }
        retval += translateOneLine(p, offset);//这一行一定是call指令

        //把a0 和 a3的值存回寄存器
        //先把开辟给参数的栈收回，然后再收回a0-a3的空间
        if(num_of_arg > 4){
            newCode = new AsmCode("addi ");
            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
            newCode->addOperand(AsmOperand(AsmOperand::IMMEDIATE, (num_of_arg - 4)* 4, -1));
            addAsmCode(*newCode);
            delete newCode;
            newCode = nullptr;
        }
        for (int i(0); i < 4;i++){
            newCode = new AsmCode("lw ");
            newCode->addOperand(AsmOperand(AsmOperand::REGISTER, 4 + i,-1));
            newCode->addOperand(AsmOperand(AsmOperand::ADDRESS, -4 * i, 29));
            AssemblyCodeList.push_back(*newCode);
            delete newCode;
            newCode = nullptr;
        }
        addSp(16);
    }

    else if((*it)->getType() == InterCode::CALL){
        //执行的流程大致为：先保存ra的的值->准备跳转->回来后把ra的值放回去,把放回值存放好；
        //其中返回值可能值直接返回到一个变量，也可能返回到一个数组的元素，也可能就是个空变量
        saveRa();
        InterCode *IC_ptr = (*it);
        CallCode *CC_ptr = (CallCode*)IC_ptr;
        Jump(CC_ptr->getFuncName(), "jal");
        retRa();
        Operand *ret_var = CC_ptr->getOperand(1);
        if(asmVarList.getVar(ret_var->getName()).getReg() == 0){
            Register &alloc_reg = Mips32.allocateReg(*this);
            alloc_reg.setVar(asmVarList.getVar(ret_var->getName()));
            asmVarList.getVar(ret_var->getName()).setReg(Mips32.getRegNumber(alloc_reg));
            moveFromRegtoReg(asmVarList.getVar(ret_var->getName()).getReg(), 2);
        }
        else{
            moveFromRegtoReg(asmVarList.getVar(ret_var->getName()).getReg(), 2);
        }
    }
    else if((*it)->getType() == InterCode::RETURN){
        //保存返回值也是，先看返回值的类型，从而决定如何获取返回值,在跳转前，记得调整栈帧
        Operand *X = (*it)->getOperand(1);
        if(X->getType() == Operand::ICONSTANT){
            ConstantOP *const_ptr = (ConstantOP *)X;
            loadConsttoReg(const_ptr->getIntVal(), 2);
        }
        else{
            if(!asmVarList.ifVarInList(X->getName())){
                moveFromRegtoReg(2, 0);
            }
            else{
                if(asmVarList.getVar(X->getName()).getReg() == 0){
                    moveFromMemtoReg(2, asmVarList.getVar(X->getName()).getAddr(), 30);
                }
                else{
                    moveFromRegtoReg(2, asmVarList.getVar(X->getName()).getReg());
                }
            }
        }
        moveFromRegtoReg(29, 30);
        moveFromMemtoReg(30, 0, 29);
        Jump("$ra", "jr");
    }

    else if((*it)->getType() == InterCode::LABEL){
        InterCode *IC_ptr = (*it);
        LabelCode *LBLC_ptr = (LabelCode *)IC_ptr;
        setLabel(LBLC_ptr->getLabelNo());
    }

    else if((*it)->getType() == InterCode::GOTO){
        InterCode *IC_ptr = (*it);
        GotoCode *GT_ptr = (GotoCode *)IC_ptr;
        Jump(turnIntToStr(GT_ptr->getDst()), "j");
    }

    else if((*it)->getType() == InterCode::COND){
        InterCode *IC_ptr = (*it);
        CondCode *Cond_ptr = (CondCode *)IC_ptr;
        Operand *op1 = Cond_ptr->getOp1();
        Operand *op2 = Cond_ptr->getOp2();
        int reg1(0), reg2(0);
        if (op1->getType() == Operand::ICONSTANT)
        {
            ConstantOP *const_ptr = (ConstantOP *)op1;
            loadConsttoReg(const_ptr->getIntVal(), 24);
            reg1 = 24;
        }
        else{
            if(!asmVarList.ifVarInList(op1->getName())){
                reg1 = 0;
            }
            else{
                if(asmVarList.getVar(op1->getName()).getReg() == 0){
                    Register &r = Mips32.allocateReg(*this);
                    r.setVar(asmVarList.getVar(op1->getName()));
                    asmVarList.getVar(op1->getName()).setReg(Mips32.getRegNumber(r));
                }
                reg1 = asmVarList.getVar(op1->getName()).getReg();
            }
        }
        if (op2->getType() == Operand::ICONSTANT)
        {
            ConstantOP *const_ptr = (ConstantOP *)op2;
            loadConsttoReg(const_ptr->getIntVal(), 25);
            reg2 = 25;
        }
        else{
            if(!asmVarList.ifVarInList(op2->getName())){
                reg2 = 0;
            }
            else{
                if(asmVarList.getVar(op2->getName()).getReg() == 0){
                    Register &r = Mips32.allocateReg(*this);
                    r.setVar(asmVarList.getVar(op2->getName()));
                    asmVarList.getVar(op2->getName()).setReg(Mips32.getRegNumber(r));
                }
                reg2 = asmVarList.getVar(op2->getName()).getReg();
            }
        }
        setCond(Cond_ptr->getRelop(), reg1, reg2, Cond_ptr->getDst());
    }
    else if((*it)->getType() == InterCode::DEC){

    }
    else if((*it)->getType() == InterCode::PARAM){
        errorRoutine("error when translateoneLine: undefined PARAM");
    }

    else if((*it)->getType() == InterCode::READ){
        InterCode *IC_ptr = (*it);
        ReadCode *rd_ptr = (ReadCode *)IC_ptr;
        Operand *X = rd_ptr->getOperand(1);
        Register &r = Mips32.allocateReg(*this);
        r.setVar(asmVarList.getVar(X->getName()));
        asmVarList.getVar(X->getName()).setReg(Mips32.getRegNumber(r));
        saveRa();
        Jump("read", "jal");
        retRa();
        moveFromRegtoReg(asmVarList.getVar(X->getName()).getReg(), 2);
    }

    else if((*it)->getType() == InterCode::WRITE){
        saveRa();
        Jump("write", "jal");
        retRa();
    }

    else if((*it)->getType() == InterCode::ASSIGN){
        
    }
    else if((*it)->getType() == InterCode::ASSIGNFROMLOC){
        
    }
    else if((*it)->getType() == InterCode::ASSIGNLOC){

    }
    else if((*it)->getType() == InterCode::ASSIGNTOLOC){
        
    }
    else if((*it)->getType() == InterCode::PLUS){

    }
    else if((*it)->getType() == InterCode::MUL){

    }
    else if((*it)->getType() == InterCode::SUB){

    }
    else if((*it)->getType() == InterCode::DIV){

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

void Assembly::moveFromMemtoReg(int regNo, int addr){
    AsmCode *code0 = new AsmCode("lw ");
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::ADDRESS, addr, 30));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::moveFromMemtoReg(int dst_regNo, int addr, int src_regNo){
    AsmCode *code0 = new AsmCode("lw ");
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, dst_regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::ADDRESS, addr, src_regNo));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::moveFromRegtoMem(int regNo, int addr){
    AsmCode *code0 = new AsmCode("sw ");
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::ADDRESS, addr, 30));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::moveFromRegtoMem(int dst_regNo, int addr, int src_regNo){
    AsmCode *code0 = new AsmCode("sw ");
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, src_regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::ADDRESS, addr, dst_regNo));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::moveFromRegtoReg(int dst_regNo, int src_regNo){
    if(dst_regNo == 0)
    {
        std::cerr << "error: move to $zero from other reg" << endl;
        exit(-1);
    }
    AsmCode *code0 = new AsmCode("move ");
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, dst_regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, src_regNo, -1));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::loadConsttoReg(int constVar, int regNo){
    AsmCode *code0(new AsmCode("li "));
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, regNo, -1));
    code0->addOperand(AsmOperand(AsmOperand::IMMEDIATE, constVar, -1));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}


void Assembly::Jump(string labelName, string jmpType){
    string jmp_instr(jmpType);
    jmp_instr.append(" ");
    jmp_instr.append(labelName);
    AsmCode *code0(new AsmCode(jmp_instr));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::setLabel(int labelNo){
    stringstream ss;
    string strLabel;
    ss << labelNo;
    ss >> strLabel;
    strLabel.append(":");
    AsmCode *code0 = new AsmCode(strLabel);
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

void Assembly::setCond(string relop, int reg1, int reg2, int dst){
    if(relop == "=="){
        setCondsubFunc("beq ", reg1, reg2, dst);
    }
    else if(relop == "!="){
        setCondsubFunc("bne ", reg1, reg2, dst);
    }
    else if(relop == ">"){
        setCondsubFunc("bgt ", reg1, reg2, dst);
    }
    else if(relop == "<"){
        setCondsubFunc("blt ", reg1, reg2, dst);
    }
    else if(relop == ">="){
        setCondsubFunc("bge ", reg1, reg2, dst);
    }
    else if(relop == "<="){
        setCondsubFunc("ble ", reg1, reg2, dst);
    }
    else {
        std::cerr << "wrong when handle cond: wrong relop type"<<endl;
        exit(-1);
    }
}

void Assembly::setCondsubFunc(string relopAsm, int reg1, int reg2, int dst){
    AsmCode *code0 = new AsmCode(relopAsm);
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, reg1, -1));
    code0->addOperand(AsmOperand(AsmOperand::REGISTER, reg2, -1));
    code0->addOperand(AsmOperand(AsmOperand::IMMEDIATE, dst, -1));
    addAsmCode(*code0);
    clearDynamicVar(code0);
}

string Assembly::turnIntToStr(int val){
    stringstream ss;
    string retval;
    ss << val;
    ss >> retval;
    return retval;
}


int Assembly::turnStrToInt(string str){
    int ret;
    stringstream ss;
    ss << str;
    ss >> ret;
    return ret;
}


void Assembly::errorRoutine(string errorInfo){
    std::cerr << errorInfo << endl;
    exit(-1);
}

void Assembly::createSpace(Operand *var, int &bp_offset){
    if(!asmVarList.ifVarInList(var->getName())){
        Variable *new_var = new Variable(0, 0, var);
        bp_offset -= new_var->getVarSize();
        new_var->setAddr(bp_offset);
        asmVarList.addVar(*new_var);
        clearDynamicVar(new_var);
    }        
}