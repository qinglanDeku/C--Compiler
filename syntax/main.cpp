#include"syntax.h"
#include"../semantic/include/SemanticAnalyze.h"
extern int yydebug;
extern void yyrestart(FILE* input_file);
extern int yyparse(void);
extern SyntaxTreeNode* TreeRoot;


int yycolumn = 1;
int yyline = 1;
int lexicalError = 0;
int syntaxError = 0;
int main(int argc, char** argv){
    if(argc <= 1) return 1;
    for(int i = 1; i < argc; i++){
        FILE* f = fopen(argv[i], "r");
        if(!f){
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        
        //yydebug = 1;        
        yyparse();
    if(lexicalError == 1 || syntaxError == 1)
        ;
    else{
    TraverseTree();
    Analyze * A= new Analyze;
    A->TraverseTree(TreeRoot);
    /*A->PrintVarSymbolTab();
    A->PrintStructType();
    A->PrintFuncSymbolTab();*/
    A->CheckFunTab();
    A->PrintSemanticError();
   // printf("????");
   // PrintTree();
    }
        
    }

    return 0;
}