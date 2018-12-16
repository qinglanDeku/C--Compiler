#include"translate.h"
#include<sstream>
#include<iostream>
#include<string>
#include<vector>
#include"../../semantic/include/SemanticAnalyze.h"
using std::cout;
using std::endl;
using std::list;
using std::string;
using std::stringstream;
using std::vector;
/*************************class Translate*********************************/
int Translate::newTemp(){
    tempVarNum += 1;
    return tempVarNum;
}

int Translate::newLabel(){
    labelNum += 1;
    return labelNum;
}

void Translate::translateTree(SyntaxTreeNode *node, Analyze *analyzeResult)
{
    if(node == NULL)
        return;
    if(GetNodeType(node) == Syntactic){
        string *tempNodeName = new string(node->NodeUnit.SU.name);
        if(*tempNodeName == "ExtDef"){
            translateExtDef(node, analyzeResult);
        }
        delete tempNodeName;
    }
    translateTree(node->NextSibling,analyzeResult);
    translateTree(node->child, analyzeResult);
}

void Translate::translateStmt(SyntaxTreeNode* stmtNode, Analyze* analyzeResult){
    if(ChildNumber(stmtNode) == 2){
        translateExp(GetChild(stmtNode, 1), analyzeResult, NULL);
    }
    else if(ChildNumber(stmtNode) == 1){
        translateCompSt(GetChild(stmtNode, 1), analyzeResult);
    }
    else if(ChildNumber(stmtNode) == 3){//return
        TemporaryOP *t1 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
        this->temporaryList.push_back(t1);
        translateExp(GetChild(stmtNode, 2), analyzeResult, t1);
        
        TemporaryOP *t2(NULL);
        turnTempAddressToVariable(t1, t2);
        RetCode *code1 = new RetCode(InterCode::RETURN, t2);
        this->IRCodeList.push_back(code1);
    }
    else if(ChildNumber(stmtNode) == 7){    //if-else
        int label1(newLabel());
        int label2(newLabel());
        int label3(newLabel());
        
        translateCond(GetChild(stmtNode, 3), label1, label2, analyzeResult);
        LabelCode *code1(new LabelCode(InterCode::LABEL, label1));
        this->IRCodeList.push_back(code1);
        translateStmt(GetChild(stmtNode, 5), analyzeResult);
        GotoCode *code2(new GotoCode(InterCode::GOTO, label3));
        this->IRCodeList.push_back(code2);
        
        LabelCode *code3(new LabelCode(InterCode::LABEL, label2));
        this->IRCodeList.push_back(code3);
        
        translateStmt(GetChild(stmtNode, 7), analyzeResult);
        LabelCode *code4(new LabelCode(InterCode::LABEL, label3));
        this->IRCodeList.push_back(code4);
        
    }
    else{
        SyntaxTreeNode *fstChild(GetChild(stmtNode, 1));
        if(fstChild->NodeUnit.LU.Lextype == LIF){// if(cond)...
            int label1(newLabel());
            int label2(newLabel());
            LabelCode *code1(new LabelCode(InterCode::LABEL, label1));
            LabelCode *code2(new LabelCode(InterCode::LABEL, label2));

            translateCond(GetChild(stmtNode, 3), label1, label2, analyzeResult);
            this->IRCodeList.push_back(code1);
            translateStmt(GetChild(stmtNode, 5), analyzeResult);
            this->IRCodeList.push_back(code2);
        }
        else{   //while(cond)...
            int label1(newLabel());
            int label2(newLabel());
            int label3(newLabel());
            LabelCode *labelCode1(new LabelCode(InterCode::LABEL, label1));
            LabelCode *labelCode2(new LabelCode(InterCode::LABEL, label2));
            LabelCode *labelCode3(new LabelCode(InterCode::LABEL, label3));
            GotoCode *gotoCode1(new GotoCode(InterCode::GOTO, label1));

            this->IRCodeList.push_back(labelCode1);
            translateCond(GetChild(stmtNode, 3), label2, label3, analyzeResult);
            this->IRCodeList.push_back(labelCode2);
            translateStmt(GetChild(stmtNode, 5), analyzeResult);
            this->IRCodeList.push_back(gotoCode1);
            this->IRCodeList.push_back(labelCode3);
        }
    }
}
void Translate::translateExp(SyntaxTreeNode *expNode, Analyze *analyzeResult, Operand *place)
{
    if(ChildNumber(expNode) == 1){//ID, #Int or #Float
        SyntaxTreeNode *fstChild(GetChild(expNode, 1));
        if(fstChild->NodeUnit.LU.Lextype == LID){//exp -> ID
            string *fstChildName(new string(GetChild(expNode, 1)->NodeUnit.LU.IDname));
            varItem *var1(analyzeResult->VariableTab.FindItem(*fstChildName));
            int var1No(0);
            VariableOP *varIR1(findVarInList(var1));
            
            if(varIR1 == NULL){//this variable no been
                var1No = analyzeResult->VariableTab.getItemNum(*fstChildName);

                if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                    var1->GetType() == STRUCT)
                {   
                    if(var1->getPlace() == 0)
                        varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                    else{
                        varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                    }
                    
                }
                else{
                    varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
                }

                this->variableList.push_back(varIR1);
            }
            if(place != NULL){
                if(varIR1->getType() == Operand::VARIABLE){
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGN, place, varIR1);
                    this->IRCodeList.push_back(code1);
                }
                else if(varIR1->getType() == Operand::ARRAY_FIRST_ELEMENT){
                    //maybe need check and set place type???
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGNLOC, place, varIR1);
                    place->setType(Operand::TEMP_ARRAY_ADRESS);
                    this->IRCodeList.push_back(code1);
                }
                else{
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGN, place, varIR1);
                    place->setType(Operand::TEMP_ARRAY_ADRESS);
                    this->IRCodeList.push_back(code1);
                }
            }

            delete fstChildName;
        }    
            
            
        else if(fstChild->NodeUnit.LU.Lextype == LINT){
            if(place != NULL){
            int constVal(fstChild->NodeUnit.LU.ival);
            ConstantOP *constIR(new ConstantOP(Operand::ICONSTANT, constVal));
            AssignCode *code1(new AssignCode(InterCode::ASSIGN, place, constIR));

            this->IRCodeList.push_back(code1);
            }
        }

        else{
            if(place != NULL){
                float constVal(fstChild->NodeUnit.LU.fval);
                ConstantOP *constIR(new ConstantOP(Operand::FCONSTANT, constVal));
                AssignCode *code1(new AssignCode(InterCode::ASSIGN, place, constIR));

                this->IRCodeList.push_back(code1);
            }
        }
    }

    else if(ChildNumber(expNode) == 2){     //three kinds
        SyntaxTreeNode *fstChild(GetChild(expNode, 1));
        if(fstChild->NodeUnit.LU.Lextype == LMINUS){
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            translateExp(GetChild(expNode, 2), analyzeResult, t1);
            ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, 0));
            BinopCode *code1;
            if (place != NULL){
                code1 = new BinopCode(InterCode::SUB, place, c1, t1);
                this->IRCodeList.push_back(code1);
            }
        }
        else if(fstChild->NodeUnit.LU.Lextype == LNOT){
            if(place != NULL){
                int label1(newLabel());
                int label2(newLabel());
                ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, 0));
                AssignCode *code0(new AssignCode(InterCode::ASSIGN, place, c1));
                this->IRCodeList.push_back(code0);
                translateCond(expNode, label1, label2, analyzeResult);
                LabelCode *Lcode1(new LabelCode(InterCode::LABEL, label1));
                ConstantOP *c2(new ConstantOP(Operand::ICONSTANT, 1));
                AssignCode *code1(new AssignCode(InterCode::ASSIGN, place, c2));
                LabelCode *Lcode2(new LabelCode(InterCode::LABEL, label2));
                this->IRCodeList.push_back(Lcode1);
                this->IRCodeList.push_back(code1);
                this->IRCodeList.push_back(Lcode2);
            }
        }
        else if(fstChild->NodeUnit.LU.Lextype == LPLUS){
            if(place != NULL)
                translateExp(GetChild(expNode, 2), analyzeResult, place);
        }
    }

    else{
        SyntaxTreeNode *secNode(GetChild(expNode, 2));
        if(secNode->NodeUnit.LU.Lextype == LLB){//array， but there are two kinds of array-visiting, in args or in stmt
            SyntaxTreeNode *exp1(GetChild(expNode, 1));
            SyntaxTreeNode *exp2(GetChild(expNode, 3));
            Analyze *tempA(new Analyze(*analyzeResult));
            varItem *exp1Var(new varItem);
            varItem *exp2Var(new varItem);//remmber using a temp var in IR to replace exp2Var
            *exp1Var = tempA->AnalyzeExp(exp1);
            translateExp(exp1, analyzeResult, place);
            /***place must be a tempvar?***/
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            translateExp(exp2, analyzeResult, t1);
            TemporaryOP *t2(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, exp1Var->getBaseTypeSize()));
            BinopCode *code1(new BinopCode(InterCode::MUL, t2, t1, c1));
            if(place != NULL){
                BinopCode *code2(new BinopCode(InterCode::PLUS, place, place, t2));
                this->IRCodeList.push_back(code1);
                this->IRCodeList.push_back(code2);
            }
            else
                this->IRCodeList.push_back(code1);
            delete exp2Var;
            delete exp1Var;
            delete tempA;
            delete exp2;
            delete exp1;
        }
        else if(secNode->NodeUnit.LU.Lextype == LDOT){//struct type
            SyntaxTreeNode *fstNode(GetChild(expNode, 1));
            SyntaxTreeNode *trdNode(GetChild(expNode, 3));
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_ARRAY_ADRESS, newTemp()));
            translateExp(fstNode, analyzeResult, t1);
            Analyze *tempA(new Analyze(*analyzeResult));
            varItem *exp1var(new varItem);
            *exp1var = tempA->AnalyzeExp(fstNode);
            string trdNodeName(trdNode->NodeUnit.LU.IDname);
            int offset(exp1var->GetStructType()->getOffset(trdNodeName));
            ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, offset));
            TemporaryOP *t2(new TemporaryOP(Operand::TEMP_ARRAY_ADRESS, newTemp()));
            BinopCode *code0(new BinopCode(InterCode::PLUS, t2, t1, c1));
            AssignCode *code1(new AssignCode(InterCode::ASSIGNFROMLOC, place, t2));
            this->IRCodeList.push_back(code0);
            this->IRCodeList.push_back(code1);
            delete exp1var;
            delete tempA;
        }

        else if(secNode->NodeUnit.LU.Lextype == LASSIGNOP){
            
            SyntaxTreeNode *fstnode(GetChild(expNode, 1));
            if(ChildNumber(fstnode) == 1){  // ID Assign
                string *fstChildName(new string(GetChild(fstnode, 1)->NodeUnit.LU.IDname));
                varItem *var1(analyzeResult->VariableTab.FindItem(*fstChildName));
                int var1No(0);
                VariableOP *varIR1(findVarInList(var1));
                if(varIR1 == NULL){//this variable no been
                    var1No = analyzeResult->VariableTab.getItemNum(*fstChildName);
                    
                    if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                        var1->GetType() == STRUCT)
                    {
                        varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                    
                    }
                    else{
                        varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
                    }

                    this->variableList.push_back(varIR1);
                }
                
                TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                translateExp(GetChild(expNode, 3), analyzeResult, t1);
                if(t1->getType() == Operand::TEMP_ARRAY_ADRESS){
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, varIR1, t1));
                    this->IRCodeList.push_back(code0);
                }
                else{
                    AssignCode *code0(new AssignCode(InterCode::ASSIGN, varIR1, t1));
                    this->IRCodeList.push_back(code0);
                }
                if(place != NULL){
                     AssignCode *code1(new AssignCode(InterCode::ASSIGN, place, varIR1));
                     this->IRCodeList.push_back(code1);
                }
               
            }
            else if(ChildNumber(fstnode) == 4){//array assign
                TemporaryOP *t1(new TemporaryOP(Operand::TEMP_ARRAY_ADRESS, newTemp()));
                translateExp(fstnode, analyzeResult, t1);
                TemporaryOP *t2(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                translateExp(GetChild(expNode, 3), analyzeResult, t2);
                TemporaryOP *t3(NULL);
                if (t2->getType() == Operand::TEMP_ARRAY_ADRESS)
                {
                    t3 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t3, t2));
                    AssignCode *code1(new AssignCode(InterCode::ASSIGNTOLOC, t1, t3));
                    this->IRCodeList.push_back(code0);
                    this->IRCodeList.push_back(code1);
                }
                else{
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNTOLOC, t1, t2));
                    this->IRCodeList.push_back(code0);
                }
                if(place != NULL){
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, place, t1));
                    this->IRCodeList.push_back(code0);
                }
            }
            else if(ChildNumber(fstnode) == 3){ //struct member assign
                TemporaryOP *t1(new TemporaryOP(Operand::TEMP_ARRAY_ADRESS, newTemp()));
                translateExp(fstnode, analyzeResult, t1);
                TemporaryOP *t2(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                translateExp(GetChild(expNode,3), analyzeResult, t2);
                TemporaryOP *t3(NULL);
                if (t2->getType() == Operand::TEMP_ARRAY_ADRESS)
                {
                    t3 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t3, t2));
                    AssignCode *code1(new AssignCode(InterCode::ASSIGNTOLOC, t1, t3));
                    this->IRCodeList.push_back(code0);
                    this->IRCodeList.push_back(code1);
                }
                else{
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNTOLOC, t1, t2));
                    this->IRCodeList.push_back(code0);
                }
                if(place != NULL){
                    AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, place, t1));
                    this->IRCodeList.push_back(code0);
                }
            }
        }

        else if(secNode->type == Syntactic && ChildNumber(expNode) == 3){//(exp)
            translateExp(secNode, analyzeResult, place);
        }
        else if(secNode->NodeUnit.LU.Lextype == LLP){//call function
            if(ChildNumber(expNode) == 3){//no args
                SyntaxTreeNode *fstNode(GetChild(expNode,1));
                string *funcName( new string(fstNode->NodeUnit.LU.IDname));
                funItem *function(analyzeResult->FunctionTab.FindItem(*funcName));
                if (*funcName == "read"){
                    if(place != NULL)
                    {
                        ReadCode *code0(new ReadCode(InterCode::READ, place));
                        this->IRCodeList.push_back(code0);
                    }
                    else{
                        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                        ReadCode *code0(new ReadCode(InterCode::READ, t1));
                        this->IRCodeList.push_back(code0);
                    }
                }
                else{
                    if(place != NULL)
                    {   
                        CallCode *code0(new CallCode(InterCode::CALL, place, *funcName));
                        this->IRCodeList.push_back(code0);
                    }
                    else{
                        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                        CallCode *code0(new CallCode(InterCode::CALL, t1, *funcName));
                        this->IRCodeList.push_back(code0);
                    }
                }
                delete funcName;
                
            }
            else{//have args
                SyntaxTreeNode *fstNode(GetChild(expNode,1));
                string *funcName( new string(fstNode->NodeUnit.LU.IDname));
                funItem *function(analyzeResult->FunctionTab.FindItem(*funcName));
                vector<Operand *> *arglist(new vector<Operand*>);
                translateArg(GetChild(expNode, 3), analyzeResult, arglist);
                if(*funcName == "write"){
                    if((*arglist)[0]->getType() == Operand::TEMP_ARRAY_ADRESS){
                        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                        AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t1, (*arglist)[0]));
                        WriteCode *code1(new WriteCode(InterCode::WRITE, t1));
                        this->IRCodeList.push_back(code0);
                        this->IRCodeList.push_back(code1);
                    }
                    else{
                        WriteCode *code0(new WriteCode(InterCode::WRITE, (*arglist)[0]));
                        this->IRCodeList.push_back(code0);
                    }
                }
                else{
                    vector<ArgCode *> templist;
                    for (int i(0); i < arglist->size(); i++)
                    {
                        if((*arglist)[i]->getType() == Operand::TEMP_ARRAY_ADRESS){
                            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
                            AssignCode *code0(new AssignCode(InterCode::ASSIGN, t1, (*arglist)[i]));
                            ArgCode *code1(new ArgCode(InterCode::ARG, t1));
                            this->IRCodeList.push_back(code0);
                            templist.push_back(code1);
                        }
                        else{
                            ArgCode *code0(new ArgCode(InterCode::ARG, (*arglist)[i]));
                            templist.push_back(code0);
                            code0 = NULL;
                        }
                    }
                    for (int i(0); i < templist.size(); i++){
                        IRCodeList.push_back(templist[i]);
                    }
                    CallCode *code1(new CallCode(InterCode::CALL, place, *funcName));
                    this->IRCodeList.push_back(code1);
                }
                delete funcName;
                
            }
        }
        else if(secNode->NodeUnit.LU.Lextype == LAND || secNode->NodeUnit.LU.Lextype ==LOR\
        || secNode->NodeUnit.LU.Lextype == LRELOP){
            if(place != NULL){
                int label1(newLabel());
                int label2(newLabel());
                ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, 0));
                AssignCode *code0(new AssignCode(InterCode::ASSIGN, place, c1));
                this->IRCodeList.push_back(code0);
                translateCond(expNode, label1, label2, analyzeResult);
                LabelCode *Lcode1(new LabelCode(InterCode::LABEL, label1));
                ConstantOP *c2(new ConstantOP(Operand::ICONSTANT, 1));
                AssignCode *code1(new AssignCode(InterCode::ASSIGN, place, c2));
                LabelCode *Lcode2(new LabelCode(InterCode::LABEL, label2));
                this->IRCodeList.push_back(Lcode1);
                this->IRCodeList.push_back(code1);
                this->IRCodeList.push_back(Lcode2);
                
            }
        }
        else{       //binop + - * /
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            TemporaryOP *t2(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            translateExp(GetChild(expNode, 1), analyzeResult, t1);
            translateExp(GetChild(expNode, 3), analyzeResult, t2);
            TemporaryOP *t3(NULL);
            TemporaryOP *t4(NULL);
            if (t1->getType() == Operand::TEMP_ARRAY_ADRESS)
            {
                t3 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
                AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t3, t1));
                this->IRCodeList.push_back(code0);
            }
            else{
                t3 = t1;
            }
            if(t2->getType() == Operand::TEMP_ARRAY_ADRESS){
                t4 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
                AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t4, t2));
                this->IRCodeList.push_back(code0);
            }
            else{
                t4 = t2;
            }
            BinopCode *code1(NULL);
            if(place == NULL)
                return;
            switch (secNode->NodeUnit.LU.Lextype)
            {
            case LPLUS:
                code1 = new BinopCode(InterCode::PLUS, place, t3, t4);
                break;
            case LMINUS:
                code1 = new BinopCode(InterCode::SUB, place, t3, t4);
                break;
            case LSTAR:
                code1 = new BinopCode(InterCode::MUL, place, t3, t4);
                break;
            case LDIV:
                code1 = new BinopCode(InterCode::DIV, place, t3, t4);
                break;
            default:
                break;
            }
            this->IRCodeList.push_back(code1);
        }
    }
}

void Translate::translateArg(SyntaxTreeNode* argsNode, Analyze* analyzeResult, vector<Operand*> *&arg_list){
    if(ChildNumber(argsNode) == 1){
        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
        translateExp(GetChild(argsNode, 1), analyzeResult, t1);
        arg_list->push_back(t1);
    }
    else{
        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
        translateExp(GetChild(argsNode, 1), analyzeResult, t1);
        arg_list->push_back(t1);
        translateArg(GetChild(argsNode, 3), analyzeResult, arg_list);
    }
}

void Translate::translateParams(SyntaxTreeNode *paramNode, Analyze *analyzeResult, vector<Operand *> *&param_list){
    if(ChildNumber(paramNode) == 1){
        SyntaxTreeNode *VarDecNode((GetChild(GetChild(paramNode, 1), 2)));
        string ParamID(GetVarDecID(VarDecNode));
        varItem *var1(analyzeResult->VariableTab.FindItem(ParamID));
        int var1No(0);
        VariableOP *varIR1(findVarInList(var1));
        if(varIR1 == NULL){//this variable no been
            var1No = analyzeResult->VariableTab.getItemNum(ParamID);
            if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                var1->GetType() == STRUCT)
            {   
                if(var1->getPlace() == 0)
                    varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                else{
                    varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                }
                
            }
            else{
                varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
            }

            this->variableList.push_back(varIR1);
        }
        param_list->push_back(varIR1);
    }
    else{
        SyntaxTreeNode *VarDecNode((GetChild(GetChild(paramNode, 1), 2)));
        string ParamID(GetVarDecID(VarDecNode));
        varItem *var1(analyzeResult->VariableTab.FindItem(ParamID));
        int var1No(0);
        VariableOP *varIR1(findVarInList(var1));
        if(varIR1 == NULL){//this variable no been
            var1No = analyzeResult->VariableTab.getItemNum(ParamID);
            if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                var1->GetType() == STRUCT)
            {   
                if(var1->getPlace() == 0)
                    varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                else{
                    varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                }
                
            }
            else{
                varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
            }

            this->variableList.push_back(varIR1);
        }
        param_list->push_back(varIR1);
        translateParams(GetChild(paramNode, 3), analyzeResult, param_list);
    }
}

void Translate::translateCond( SyntaxTreeNode* condNode, int trueLable, int falseLable, Analyze* analyzeResult){
    if(ChildNumber(condNode) == 2){
        translateCond(GetChild(condNode, 2), falseLable, trueLable, analyzeResult);
    }
    else if(ChildNumber(condNode) == 3){
        if(GetChild(condNode,2)->NodeUnit.LU.Lextype == LRELOP){
            SyntaxTreeNode *secNode(GetChild(condNode, 2));
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            TemporaryOP *t2(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            translateExp(GetChild(condNode, 1), analyzeResult, t1);
            translateExp(GetChild(condNode, 3), analyzeResult, t2);
            TemporaryOP *t3(NULL);
            TemporaryOP *t4(NULL);
            turnTempAddressToVariable(t1, t3);
            turnTempAddressToVariable(t2, t4);
            string relopStr(GetChild(condNode,2)->NodeUnit.LU.IDname);
            CondCode *code0(new CondCode(InterCode::COND, t3, t4, relopStr, trueLable));
            GotoCode *code1(new GotoCode(InterCode::GOTO, falseLable));
            this->IRCodeList.push_back(code0);
            this->IRCodeList.push_back(code1);
        }
        else if(GetChild(condNode,2)->NodeUnit.LU.Lextype == LAND){
            int label1(newLabel());
            translateCond(GetChild(condNode, 1), label1, falseLable, analyzeResult);
            LabelCode *code0(new LabelCode(InterCode::LABEL, label1));
            this->IRCodeList.push_back(code0);
            translateCond(GetChild(condNode, 3), trueLable, falseLable, analyzeResult);
        }
        else if(GetChild(condNode,2)->NodeUnit.LU.Lextype == LOR){
            int lable1(newLabel());
            translateCond(GetChild(condNode, 1), trueLable, lable1, analyzeResult);
            LabelCode *code0(new LabelCode(InterCode::LABEL, lable1));
            this->IRCodeList.push_back(code0);
            translateCond(GetChild(condNode, 3), trueLable, falseLable, analyzeResult);
        }
        else{
            TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
            TemporaryOP *t2(NULL);
            translateExp(condNode, analyzeResult, t1);
            turnTempAddressToVariable(t1, t2);
            ConstantOP *c1(new ConstantOP(Operand::ICONSTANT, 0));
            CondCode *code0(new CondCode(InterCode::COND, t2, c1, "!=", trueLable));
            GotoCode *code1(new GotoCode(InterCode::GOTO, falseLable));
            this->IRCodeList.push_back(code0);
            this->IRCodeList.push_back(code1);
        }
    }
}

void Translate::translateExtDef(SyntaxTreeNode *extDefNode, Analyze *analyzeResult){
    if(ChildNumber(extDefNode) == 3){
        string secNodeName(GetChild(extDefNode, 2)->NodeUnit.SU.name);
        if (secNodeName == "FunDec" && GetChild(extDefNode, 3)->type == Syntactic)
        {
            SyntaxTreeNode *funcNode(GetChild(extDefNode, 2));
            string funcName(GetChild(funcNode, 1)->NodeUnit.LU.IDname);
            FuncDecCode *code0(new FuncDecCode(InterCode::FUNC, &funcName));
            this->IRCodeList.push_back(code0);
            if (ChildNumber(funcNode) == 3)
                ;
            else{
                SyntaxTreeNode *ParamListNode(GetChild(funcNode, 3));
                vector<Operand *> *paramList(new vector<Operand *>);
                translateParams(ParamListNode, analyzeResult, paramList);
                for (int i(paramList->size() - 1); i >= 0; i--){
                    ParamCode *code1(new ParamCode(InterCode::PARAM, (*paramList)[i]));
                    this->IRCodeList.push_back(code1);
                    code0 = NULL;
                }
                delete paramList;
            }
            translateCompSt(GetChild(extDefNode, 3), analyzeResult);
        }
        else if(secNodeName == "ExtDecList"){
            SyntaxTreeNode *ExtDecListNode(GetChild(extDefNode, 2));
            SyntaxTreeNode *varDecNode(GetChild(ExtDecListNode, 1));
            while(ChildNumber(ExtDecListNode) != 1){
                string IDName(GetVarDecID(varDecNode));
                
                varItem *var1(analyzeResult->VariableTab.FindItem(IDName));
                int IDSize(var1->getSize());
                int var1No(0);
                VariableOP *varIR1(findVarInList(var1));
                if(varIR1 == NULL){//this variable no been
                    var1No = analyzeResult->VariableTab.getItemNum(IDName);
                    if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                        var1->GetType() == STRUCT)
                    {   
                        if(var1->getPlace() == 0)
                            varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                        else{
                            varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                        }
                        
                    }
                    else{
                        varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
                    }

                    this->variableList.push_back(varIR1);
                }
                DecCode *code0(new DecCode(InterCode::DEC, varIR1, IDSize));
                this->IRCodeList.push_back(code0);
                code0 = NULL;
                ExtDecListNode = GetChild(ExtDecListNode, 3);
                varDecNode = GetChild(ExtDecListNode, 1);
            }
            string IDName(GetVarDecID(varDecNode));    
            varItem *var1(analyzeResult->VariableTab.FindItem(IDName));
            int IDSize(var1->getSize());
            int var1No(0);
            VariableOP *varIR1(findVarInList(var1));
            if(varIR1 == NULL){//this variable no been
                var1No = analyzeResult->VariableTab.getItemNum(IDName);
                if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                    var1->GetType() == STRUCT)
                {   
                    if(var1->getPlace() == 0)
                        varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                    else{
                        varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                    }
                    
                }
                else{
                    varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
                }

                this->variableList.push_back(varIR1);
            }
            DecCode *code0(new DecCode(InterCode::DEC, varIR1, IDSize));
            this->IRCodeList.push_back(code0);
            code0 = NULL;

        }
    }


}


void Translate::translateDec(SyntaxTreeNode *DecNode, Analyze *analyzeResult){
    SyntaxTreeNode *varDecNode(GetChild(DecNode, 1));
    string IDName(GetVarDecID(varDecNode));
    if(ChildNumber(DecNode) == 1){
        varItem *var1(analyzeResult->VariableTab.FindItem(IDName));
        if(var1->GetType() == INT || var1->GetType() == FLOAT)
            ;
        else{
            int IDSize(var1->getSize());
            int var1No(0);
            VariableOP *varIR1(findVarInList(var1));
            if(varIR1 == NULL){//this variable no been
                var1No = analyzeResult->VariableTab.getItemNum(IDName);
                if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                    var1->GetType() == STRUCT)
                {   
                    if(var1->getPlace() == 0)
                        varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                    else{
                        varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                    }
                    
                }
                else{
                    varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
                }

                this->variableList.push_back(varIR1);
            }
            DecCode *code0(new DecCode(InterCode::DEC, varIR1, IDSize));
            this->IRCodeList.push_back(code0);
            code0 = NULL;
        }
    }
    else{
        varItem *var1(analyzeResult->VariableTab.FindItem(IDName));
        int var1No(0);
        VariableOP *varIR1(findVarInList(var1));
        if(varIR1 == NULL){//this variable no been
            var1No = analyzeResult->VariableTab.getItemNum(IDName);
            if (var1->GetType() == INTARRAY || var1->GetType() == FLOATARRAY || var1->GetType() == STRUCTARRAY ||
                var1->GetType() == STRUCT)
            {   
                if(var1->getPlace() == 0)
                    varIR1 = new VariableOP(Operand::ARRAY_FIRST_ELEMENT, var1No, var1);
                else{
                    varIR1 = new VariableOP(Operand::ARRAY_ADRESS, var1No, var1);
                }
                
            }
            else{
                varIR1 = new VariableOP(Operand::VARIABLE, var1No, var1);
            }

            this->variableList.push_back(varIR1);
        }
        SyntaxTreeNode *expNode(GetChild(DecNode, 3));
        TemporaryOP *t1(new TemporaryOP(Operand::TEMP_VARIABLE, newTemp()));
        TemporaryOP *t2(NULL);
        translateExp(expNode, analyzeResult, t1);
        turnTempAddressToVariable(t1, t2);
        AssignCode *code0(new AssignCode(InterCode::ASSIGN, varIR1, t2));
        this->IRCodeList.push_back(code0);
    }
}
    
void Translate::translateCompSt(SyntaxTreeNode *compstNode, Analyze *analyzeResult){
    SyntaxTreeNode *DefListNode(GetChild(compstNode, 2));
    SyntaxTreeNode *StmtListNode(GetChild(compstNode, 3));
    
    translateDefList(DefListNode, analyzeResult);
    translateStmtList(StmtListNode, analyzeResult);
}

void Translate::translateStmtList(SyntaxTreeNode *StmtListNode, Analyze *analyzeResult){
    SyntaxTreeNode *StmtNode(NULL);
    while(ChildNumber(StmtListNode) == 2){
        StmtNode = GetChild(StmtListNode, 1);
        translateStmt(StmtNode, analyzeResult);
        StmtListNode = GetChild(StmtListNode, 2);
    }
}

void Translate::translateDefList(SyntaxTreeNode *DefListNode, Analyze *analyzeResult){
    SyntaxTreeNode *DefNode(NULL);
    while(ChildNumber(DefListNode) == 2){
        DefNode = GetChild(DefListNode, 1);
        translateDef(DefNode, analyzeResult);
        DefListNode = GetChild(DefListNode, 2);
    }
}

void Translate::translateDef(SyntaxTreeNode *DefNode, Analyze *analyzeResult){
    SyntaxTreeNode *DecListNode(GetChild(DefNode, 2));
    SyntaxTreeNode *DecNode(NULL);
    while(ChildNumber(DecListNode) != 1){
        DecNode = GetChild(DecListNode, 1);
        translateDec(DecNode, analyzeResult);
        DecListNode = GetChild(DecListNode, 3);
    }
    DecNode = GetChild(DecListNode, 1);
    translateDec(DecNode, analyzeResult);
}

void Translate::printCodeList(){
    list<InterCode *>::iterator it(IRCodeList.begin());
    while (it != IRCodeList.end())
    {
        cout << (*it)->getCode() << endl;
        it++;
    }
}