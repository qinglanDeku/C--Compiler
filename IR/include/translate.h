#pragma once
#include"InterCode.h"
#include"../../semantic/include/SemanticAnalyze.h"
class Translate{
public:
    Translate():tempVarNum(0), labelNum(0){}
    void printCodeList();
    
    void translateTree(SyntaxTreeNode* root, Analyze* analyzeResult);
    void translateExp( SyntaxTreeNode* expNode, Analyze* analyzeResult, Operand *place);
    void translateCond( SyntaxTreeNode* condNode, int trueLable, int falseLable, Analyze* analyzeResult);
    void translateStmt( SyntaxTreeNode* stmtNode, Analyze* analyzeResult);
    void translateArg( SyntaxTreeNode* argsNode, Analyze* analyzeResult, vector<string> *arg_list);
    void translateCompSt(SyntaxTreeNode *compstNode, Analyze *analyzeResult);

  private:
    list<InterCode*> IRCodeList;
    list<VariableOP*> variableList;
    list<TemporaryOP *> temporaryList;
    int tempVarNum;
    int labelNum;
    int newTemp(); //remember to delete the dynamic return val before not using it
    int newLabel();
    VariableOP* findVarInList(varItem* item);
};