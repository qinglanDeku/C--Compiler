#include"InterCode.h"
#include"../../semantic/include/SemanticAnalyze.h"
class Translate{
public:
    void printCodeList();
    
    void translateTree(const SyntaxTreeNode* root, const Analyze* analyzeResult);
    void translateExp(const SyntaxTreeNode* expNode, const VarSymbolTab* varTable, string place);
    void translateCond(const SyntaxTreeNode* condNode, int trueLable, int falseLable, const VarSymbolTab* varTable);
    void translateStmt(const SyntaxTreeNode* stmtNode, const VarSymbolTab* varTable);
    void translateArg(const SyntaxTreeNode* argsNode, const VarSymbolTab* varTable, vector<string> *arg_list);
    
private:
    list<InterCode*> IRCodeList;
};