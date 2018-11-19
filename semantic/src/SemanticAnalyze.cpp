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
    /*judge this Specifier a struct or a base type*/
    if(ChildNumber(ExtDefNode) == 2){
        /*undner most circumstances, using this production for 
        define a type of struct or declare a type of struct*/
        if(GetChild(specf, 1)->type == Lexical)
            /*nonsene*/;
        
        else{
            SyntaxTreeNode* structSpecf = specf->child;     //structp = StructSpecifier

            /*this is just a declaration, and need to check the structTab, 
                ensure the struct has been definition.If not, need to define after*/
            if(ChildNumber(structSpecf) == 2){    
                string StructName(GetChild(structSpecf,2)->child->NodeUnit.LU.IDname);
                if(StructTab.FindItem(StructName) == NULL){
                    /*struct type dec but not def*/
                    structItem newItem(StructName, structSpecf->lineno, -1);
                    StructTab.AddItem(newItem);
                }
                else ;
            
            }   

            /*truely define a struct tytpe*/
            else{
                if(GetChild(structSpecf,2)->type == Empty)
                   /*only defining noname struct type without variable has 
                   nonsense*/ ;
                else{
                    string structName(GetChild(GetChild(structSpecf,2),1)->NodeUnit.LU.IDname);
                    if(StructTab.FindItem(structName) == NULL){
                        /*first define*/
                        structItem newStructItem(structName, structSpecf->lineno, structSpecf->lineno);
                        SyntaxTreeNode* DefList = GetChild(structSpecf, 4);
                        SyntaxTreeNode* Def = GetChild(DefList, 1);
                        while(DefList->type != Empty){
                            /*for every member def, first ensure Specifier,
                                then dec*/
                            SyntaxTreeNode* DefSpecf = GetChild(Def, 1);
                            if(GetChild(DefSpecf, 1)->type == Lexical){
                            /*base type member*/
                                SyntaxTreeNode* DecList(GetChild(Def, 2));
                                SyntaxTreeNode* Dec(GetChild(DecList, 1));
                                string StrMemberType(GetChild(DefSpecf,1)->NodeUnit.LU.IDname);
                                TYPE MemberType;
                                if(ChildNumber(Dec) == 3){
                                    /*Error type 15 ,initialize member var*/
                                    /*Error but still add this memeber into varList as if Right
                                    Firstly add Error, then Add member*/
                                    
                                    int CountDimension(0);
                                    SyntaxTreeNode* VarDec = GetChild(Dec, 1);
                                    SyntaxTreeNode* VarName = GetChild(VarDec, 1);
                                    while(ChildNumber(VarDec) != 1){
                                        VarDec = GetChild(VarDec, 1);
                                        VarName = GetChild(VarDec, 1);
                                        CountDimension += 1;
                                    }
                                    string MemName(VarName->NodeUnit.LU.IDname);
                                    SemanticError newError(Dec->lineno, MemName, 15);
                                    ErrorList.AddError(newError);
                                    if(CountDimension == 0)
                                        MemberType = (StrMemberType == "int"?INT:FLOAT);
                                    else
                                        MemberType = (StrMemberType == "int"?INTARRAY:FLOATARRAY);
                                    /*befor add, check it*/
                                    if(newStructItem.GetMember(MemName) != NULL){
                                        /*already add error type 15*/;
                                    }
                                    else if(VariableTab.FindItem(MemName) != NULL){
                                        /*error type 3*/
                                        
                                    }
                                }
                                while(ChildNumber(DecList) != 1);
                            }
        
                            else{
                            /*struct type member, unfinished*/
                            }

                        StructTab.AddItem(newStructItem);
                    }
                }
            }
        }
    }







    if(specf->child->type == Syntactic){    //this is a struct Specifier
        SyntaxTreeNode* structSpecf = specf->child;     //structp = StructSpecifier
        if(ChildNumber(structSpecf) == 2){
            /*this may be just a declaration, and need to check the structTab, 
            ensure the struct has been definition.If not, need to define after*/
            string StructName(GetChild(structSpecf,2)->child->NodeUnit.LU.IDname);
            if(StructTab.FindItem(StructName) == NULL){
                structItem newItem(StructName, structSpecf->lineno, -1);
                StructTab.AddItem(newItem);
            }
            else if(StructTab.FindItem(StructName)->MemberNum() == 0){
                /*using undefined struct type*/
                SemanticError NewError(structSpecf->lineno, StructName, 17);
                ErrorList.AddError(NewError);
            }
            else{
                /*defined variable or just a non-use declaration*/
                //if(ChildNumber(ExtDefNode) == )
                
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
            structItem newItem(StructName, structSpecf->lineno, -1);

            StructTab.AddItem(newItem);
            
        }
    }

}

