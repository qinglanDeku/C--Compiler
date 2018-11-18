#include"SemanticAnalyze.h"

/*class Analyze*/

void Analyze::TraverseTree(SyntaxTreeNode* node){
    if(node == NULL)
        return;
    if(node->type == 0){
        string nodeName(node->NodeUnit.SU.name);
        if(nodeName == "ExtDef" || nodeName == "Def")
            AnalyzeDef(node);
        else if(nodeName == "Exp")
            AnalyzeExp(node);
    }
    TraverseSubTree(node->child);
    TraverseSubTree(node->NextSibling);
}

void Analyze::AnalyzeDef(SyntaxTreeNode* node){
    SyntaxTreeNode* p = node->child;
    string SpecifierName;       
    TYPE DefType;       //retval when function def, type when variable def
    /*judge this Specifier a struct or a base type*/
    if(p->child->type == 0){    //this is a struct Specifier
        SpecifierName = p->child->NodeUnit.SU.name;
        SyntaxTreeNode* structP = p->child;     //structp = StructSpecifier
        if(ChildNumber(structP) == 2){
            /*this is just a declaration, need to complete definition*/
            string StructName(GetChild(structP,2)->child->NodeUnit.SU.name);
            structItem newItem(StructName, structP->lineno);
            StructTab.AddItem(newItem);
            
        }

        else{
            /*this is a struct definition, but maybe has no name!*/
            
        }
    }

}

