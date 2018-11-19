#include"SemanticError.h"
#include"Symboltab.h"
#include"../../syntax/syntax.h"

class Analyze{
public:
    void TraverseTree(SyntaxTreeNode* Node);

private:
    void AnalyzeExtDef(SyntaxTreeNode* Node);
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeExp(SyntaxTreeNode* Node);
    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
};