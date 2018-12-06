#include"translate.h"
#include<sstream>
using std::stringstream;
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
        if (*tempNodeName == "Stmt"){
            translateStmt(node, analyzeResult);
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
        RetCode *code1 = new RetCode(InterCode::RETURN, t1);
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

void Translate::translateExp(SyntaxTreeNode* expNode, Analyze* analyzeResult, Operand* place){
    if(ChildNumber(expNode) == 1){//ID, #Int or #Float
        SyntaxTreeNode *fstChild(GetChild(expNode, 1));
        if(fstChild->NodeUnit.LU.Lextype == LID){//exp -> ID
            string *fstChildName(new string(GetChild(expNode, 1)->NodeUnit.LU.IDname));
            varItem *var1(analyzeResult->VariableTab.FindItem(*fstChildName));
            int var1No(0);
            VariableOP *varIR1(findVarInList(var1));
            
            if(varIR1 == NULL){
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
            if(place != NULL){
                if(varIR1->getType() == Operand::VARIABLE){
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGN, place, varIR1);
                    
                    this->IRCodeList.push_back(code1);
                }
                else if(varIR1->getType() == Operand::ARRAY_FIRST_ELEMENT){
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGNLOC, place, varIR1);

                    this->IRCodeList.push_back(code1);
                }
                else{
                    AssignCode *code1 = new AssignCode(InterCode::ASSIGN, place, varIR1);

                    this->IRCodeList.push_back(code1);
                }
            }


            delete fstChildName;
        }    
            
            
        else if(1)
            ;
    }
}