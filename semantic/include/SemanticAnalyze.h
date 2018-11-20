#include"SemanticError.h"
#include"Symboltab.h"
#include"../../syntax/syntax.h"

class Analyze{
public:
    void TraverseTree(SyntaxTreeNode* Node);

private:
#define DefinedStructType 1
#define UnDefinedStructType 0
    void AnalyzeExtDef(SyntaxTreeNode* Node);
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeExp(SyntaxTreeNode* Node);
    structItem* AnlzStruct(SyntaxTreeNode* StructSpecf, structItem* OwnerStruct, int ifDef);
    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
};