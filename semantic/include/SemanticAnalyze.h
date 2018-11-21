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
    void AnalyzeExtDef(SyntaxTreeNode* Node);
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeExp(SyntaxTreeNode* Node);
    bool AnlzSpecf(SyntaxTreeNode* DefSpecf, string &StrType, structItem* &structType);
    structItem* AnlzStruct(SyntaxTreeNode* StructSpecf, structItem* OwnerStruct, int ifDef);
    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
};