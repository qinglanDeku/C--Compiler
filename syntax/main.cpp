#include"syntax.h"
#include"../semantic/include/SemanticAnalyze.h"
#include"../IR/include/translate.h"
#include"../assembly/include/Assembly.h"
#include<iostream>
#include<string>
//#define DEBUG 1
extern "C" int lexyy();
extern int yydebug;
extern void yyrestart(FILE* input_file);
extern int yyparse(void);
extern SyntaxTreeNode* TreeRoot;
using std::string;

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
            A->CheckFunTab();
            A->PrintSemanticError();
            if(A->ErrorList.size() != 0)
                break;
            /*A->PrintVarSymbolTab();
            A->PrintStructType();*/
            Translate *T = new Translate;
            T->translateTree(TreeRoot, A);
            //T->printVariable();
            //T->printCodeList();
#ifdef DEBUG
            std::cout << "finish interCode, ready for ASM" << std::endl;
#endif
            string *filename(nullptr);
            if (argc == 3)
                filename = new string(argv[2]);
            Assembly *Asm = new Assembly(T->getCodeList(), T->getVarList(), filename);
            Asm->produceAssembly();
            delete filename;
            delete Asm;
            delete T;
            delete A;
#ifdef DEBUG
            std::cout << "finish ASM" << std::endl;
#endif
            //Asm->printAssembly();
            break;
        }
    }

    return 0;
}