#pragma once
#include"InterCode.h"
#include"../../semantic/include/SemanticAnalyze.h"
class Translate{
public:
    Translate():tempVarNum(0), labelNum(0), visitArray(false){}
    void printCodeList();
    
    void translateTree(SyntaxTreeNode* root, Analyze* analyzeResult);
    void translateExp( SyntaxTreeNode* expNode, Analyze* analyzeResult, Operand *place);
    void translateCond( SyntaxTreeNode* condNode, int trueLable, int falseLable, Analyze* analyzeResult);
    void translateStmt( SyntaxTreeNode* stmtNode, Analyze* analyzeResult);
    void translateArg( SyntaxTreeNode* argsNode, Analyze* analyzeResult, vector<Operand*> *&arg_list);
    void translateCompSt(SyntaxTreeNode *compstNode, Analyze *analyzeResult);
    void translateExtDef(SyntaxTreeNode *extDefNode, Analyze *analyzeResult);
    void translateParams(SyntaxTreeNode *argsNode, Analyze *analyzeResult, vector<Operand *> *&param_list);
    void translateDec(SyntaxTreeNode *DecNode, Analyze *analyzeResult);
    void translateDefList(SyntaxTreeNode *DefListNode, Analyze *analyzeResult);
    void translateStmtList(SyntaxTreeNode *StmtListNode, Analyze *analyzeResult);
    void translateDef(SyntaxTreeNode *DefNode, Analyze *analyzeResult);
    static string turnoverRelopStr(const string &relopStr);
    void printVariable();
    const list<InterCode *> &getCodeList() { return IRCodeList; }
    const list<VariableOP *> &getVarList() { return variableList; }

  private:
    list<InterCode*> IRCodeList;
    list<VariableOP*> variableList;
    list<TemporaryOP *> temporaryList;
    int tempVarNum;
    int labelNum;
    int newTemp(); //remember to delete the dynamic return val before not using it
    int newLabel();
    bool visitArray; //visit array in args or stmt;0---stmt 1 ---args
    VariableOP *findVarInList(varItem *item){
      list<VariableOP *>::iterator it(variableList.begin());
      while(it != variableList.end()){
        if((*it)->getSymbolTabItem() == item){
          return *it;
        }
        it++;
      }
      return NULL;
    }
    void turnTempAddressToVariable(TemporaryOP*&t1, TemporaryOP*&t2){
      if(t1->getType() == Operand::TEMP_ARRAY_ADRESS){
        t2 = new TemporaryOP(Operand::TEMP_VARIABLE, newTemp());
        AssignCode *code0(new AssignCode(InterCode::ASSIGNFROMLOC, t2, t1));
        this->IRCodeList.push_back(code0);
      }
      else{
        t2 = t1;
      }
    }
    string GetVarDecID(SyntaxTreeNode *varDecNode){
      SyntaxTreeNode *temp(varDecNode);
      while(ChildNumber(temp) != 1){
        temp = GetChild(temp, 1);
      }
      SyntaxTreeNode *IDNode(GetChild(temp, 1));
      string retval(IDNode->NodeUnit.LU.IDname);
      return retval;
    }
};