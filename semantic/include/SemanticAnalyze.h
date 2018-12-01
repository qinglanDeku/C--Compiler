#pragma once
#include"SemanticError.h"
#include"Symboltab.h"
#include"../../syntax/syntax.h"

class Analyze{
public:
    void TraverseTree(SyntaxTreeNode* Node);
    void PrintSemanticError(){
        ErrorList.PrintErrorList();
    }
    void PrintVarSymbolTab(){
        VariableTab.PrintTab();
    }
    void PrintFuncSymbolTab(){
        FunctionTab.PrintTab();
    }
    void PrintStructType(){
        StructTab.PrintList();
    }
    void CheckFunTab();

    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
private:
#define DefinedStructType 1
#define UnDefinedStructType 0
#define AnlzDec 0
#define AnlzDef 1
#define GLOBAL 0
#define LOCAL 1     //only analyze varaible in arglist of function
    void AnalyzeExtDef(SyntaxTreeNode* Node);
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeStmt(SyntaxTreeNode* StmtNode, funItem* FatherFunc);
    varItem AnalyzeExp(SyntaxTreeNode* ExpNode);
    void AnlzFunc(SyntaxTreeNode* FuncNode, TYPE retType, structItem* structType);
    void AnlzFuncArgList(SyntaxTreeNode* ArgListNode, funItem& func, char AnlyStyle);
    vector<varItem>* AnalyzeArgs(SyntaxTreeNode* ArgsNode);
    bool AnlzSpecf(SyntaxTreeNode* DefSpecf, string &StrType, structItem* &structType);
    void AnalyzeCompSt(SyntaxTreeNode* CompStNode, funItem* FatherFunc);
    structItem* AnlzStruct(SyntaxTreeNode* StructSpecf, structItem* OwnerStruct, int ifDef);
    void setRetType(string &strType, TYPE &type){
        if(strType == "int")
            type = INT;
        else if(strType == "float")
            type = FLOAT;
        else
            type = STRUCT;
    }

    varItem* AnalyzeVarDec(SyntaxTreeNode *varDecNode, string &StrType, \
    structItem* structType, int VariableType);




};