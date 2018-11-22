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
private:
#define DefinedStructType 1
#define UnDefinedStructType 0
#define AnlzDec 0
#define AnlzDef 1
    void AnalyzeExtDef(SyntaxTreeNode* Node);
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeStmt(SyntaxTreeNode* StmtNode);
    void AnalyzeExp(SyntaxTreeNode* Node);
    void AnlzFunc(SyntaxTreeNode* FuncNode, TYPE retType, structItem* structType);
    void AnlzFuncArgList(SyntaxTreeNode* ArgListNode, funItem& func, char AnlyStyle);
    bool AnlzSpecf(SyntaxTreeNode* DefSpecf, string &StrType, structItem* &structType);
    void AnalyzeCompSt(SyntaxTreeNode* CompStNode);
    structItem* AnlzStruct(SyntaxTreeNode* StructSpecf, structItem* OwnerStruct, int ifDef);
    void setRetType(string &strType, TYPE &type){
        if(strType == "int")
            type = INT;
        else if(strType == "float")
            type = FLOAT;
        else
            type = STRUCT;
    }
    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
};