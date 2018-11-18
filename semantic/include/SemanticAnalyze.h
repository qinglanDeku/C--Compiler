#include"SemanticError.h"
#include"Symboltab.h"
#include"../../syntax/syntax.h"

class Analyze{
public:
    void TraverseTree(SyntaxTreeNode* Node);

private:
    void AnalyzeDef(SyntaxTreeNode* Node);
    void AnalyzeDec(SyntaxTreeNode* Node);
    void AnalyzeExp(SyntaxTreeNode* Node);
    VarSymbolTab VariableTab;
    FuncSymbolTab FunctionTab;
    structTab StructTab;
    SemanticErrorList ErrorList;
};