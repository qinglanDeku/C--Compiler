#include"SemanticAnalyze.h"

/*class Analyze*/

void Analyze::TraverseTree(SyntaxTreeNode* node){
    if(node == NULL)
        return;
    if(node->type == 0){
        string nodeName(node->NodeUnit.SU.name);
        if(nodeName == "ExtDef")
            AnalyzeExtDef(node);
        else if(nodeName == "Def")
            AnalyzeDef(node);
        else if(nodeName == "Exp")
            AnalyzeExp(node);
    }
    TraverseSubTree(node->child);
    TraverseSubTree(node->NextSibling);
}

void Analyze::AnalyzeExtDef(SyntaxTreeNode* ExtDefNode){
    SyntaxTreeNode* specf = ExtDefNode->child;        //p = Specifier
    string SpecifierName;       
    TYPE DefType;       //retval when function def, type when variable def
    /*judge this Specifier a struct or a base type*/
    if(specf->child->type == Syntactic){    //this is a struct Specifier
        SpecifierName = specf->child->NodeUnit.SU.name;
        SyntaxTreeNode* structSpecf = specf->child;     //structp = StructSpecifier
        if(ChildNumber(structSpecf) == 2){
            /*this may be just a declaration, and need to check the structTab, 
            ensure the struct has been definition.If not, need to define after*/
            string StructName(GetChild(structSpecf,2)->child->NodeUnit.LU.IDname);
            if(StructTab.FindItem(StructName) == NULL){
                structItem newItem(StructName, structSpecf->lineno);
                StructTab.AddItem(newItem);
            }
            else if(StructTab.FindItem(StructName)->MemberNum() == 0){
                /*using undefined struct type*/
                SemanticError NewError(structSpecf->lineno, StructName, 17);
                ErrorList.AddError(NewError);
            }
            else{
                /*defined variable or just a non-use declaration*/
                if(ChildNumber(ExtDefNode) == )
                
            }
            
        }

        else{
            /*this is a struct definition, but maybe has no name!*/
            SyntaxTreeNode *nameNode = GetChild(structSpecf, 2);
            string StructName;
            if(nameNode->type == Empty)
                StructName = "\0";
            else
                StructName = GetChild(nameNode, 1)->NodeUnit.LU.IDname;
            structItem newItem(StructName, structSpecf->lineno);

            StructTab.AddItem(newItem);
            
        }
    }

}

