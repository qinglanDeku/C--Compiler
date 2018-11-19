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

//void Analyze::AnlzStructMem(SyntaxTreeNode* SubStructSpecf, structItem* OwnerStruct);

void Analyze::AnalyzeExtDef(SyntaxTreeNode* ExtDefNode){
    SyntaxTreeNode* specf = ExtDefNode->child;        //p = Specifier      
    /*judge this Specifier a struct or a base type*/
    if(ChildNumber(ExtDefNode) == 2){
    /*undner most circumstances, using this production for 
    define a type of struct or declare a type of struct*/
        if(GetChild(specf, 1)->type == Lexical)
            /*nonsene
            this kind of ExtDef is like
            "int;"
            "float;"*/;
        
        else{
            SyntaxTreeNode* structSpecf = GetChild(specf,1);     //structSpecf = StructSpecifier

            if(ChildNumber(structSpecf) == 2){    
            /*this is just a declaration, and need to check the structTab, 
            ensure the struct has been definition.If not, need to define after*/
                string StructName(GetChild(structSpecf,2)->child->NodeUnit.LU.IDname);
                if(StructTab.FindItem(StructName) == NULL){
                /*struct type dec but not def*/
                    structItem newItem(StructName, structSpecf->lineno, -1);
                    StructTab.AddItem(newItem);
                }
                else ;
            
            }   

            
            else{
            /*truely define a struct tytpe*/
                string structName;
                if(GetChild(structSpecf,2)->type == Empty){
                /*No name struct type ,can't be used after*/
                    structName = "";
                }
                else 
                    structName = string(GetChild(GetChild(structSpecf,2),1)->NodeUnit.LU.IDname);
                
                
                if(structName == "" || StructTab.FindItem(structName) == NULL ){
                /*not exist in the StructTab, first define*/
                    structItem newStructItem(structName, structSpecf->lineno, structSpecf->lineno);
                    SyntaxTreeNode* DefList(GetChild(structSpecf, 4));
                    SyntaxTreeNode* Def(NULL);
                    /*begin check member variable*/
                    while(DefList->type != Empty){
                    /*for every member def, first ensure Specifier,
                    then dec*/
                        Def = GetChild(DefList, 1);
                        SyntaxTreeNode* DefSpecf = GetChild(Def, 1);
                        string StrMemberType;
                        TYPE MemberType;
                        structItem* MembStructType(NULL);
                        if(GetChild(DefSpecf, 1)->type == Lexical){
                            /*base type*/
                            StrMemberType = string(GetChild(DefSpecf,1)->NodeUnit.LU.IDname);
                        }
                        else{
                            MembStructType = AnlzStructMem(DefSpecf, &newStructItem);
                        }

                        SyntaxTreeNode* DecList(GetChild(Def, 2));
                        SyntaxTreeNode* Dec(GetChild(DecList, 1));            
                        int CountDimension(0);
                        SyntaxTreeNode* VarDec(GetChild(Dec, 1));
                        SyntaxTreeNode* VarName(GetChild(VarDec, 1));
                        while(ChildNumber(VarDec) != 1){
                            VarDec = GetChild(VarDec, 1);
                            VarName = GetChild(VarDec, 1);
                            CountDimension += 1;
                        }
                        string MemName(VarName->NodeUnit.LU.IDname);
                        if(ChildNumber(Dec) == 3){
                            /*Error type 15 ,initialize member var*/
                            /*Error but still add this memeber into varList as if Right
                            Firstly add Error, then Add member*/
                            SemanticError newError(Dec->lineno, MemName, 15);
                            ErrorList.AddError(newError);
                        }
                        if(CountDimension == 0){
                        /*this member variable is not an array*/
                            if(MembStructType == NULL){
                            /*this Member's type is not struct */ 
                                MemberType = (StrMemberType == "int"?INT:FLOAT);
                            }
                            else
                                MemberType = STRUCT;
                        }
                        else{
                        /*this member variable is an array*/
                            if(MembStructType == NULL)
                                MemberType = (StrMemberType == "int"?INTARRAY:FLOATARRAY);
                            else
                                MemberType = STRUCTARRAY; 

                        }    
                        /*before add, check it*/
                        if(newStructItem.GetMember(MemName) != NULL){
                            /*error type15, redefine of a struct member*/
                            SemanticError newError(Dec->lineno, MemName, 15);
                            ErrorList.AddError(newError);
                        }
                        else if(VariableTab.FindItem(MemName) != NULL){
                            /*error type 3*/
                            SemanticError newError(Dec->lineno, MemName, 3);
                            ErrorList.AddError(newError);
                        }
                        else{
                                /*no redefine*/
                            varItem newMemVar(MemName, MemberType, Dec->lineno, CountDimension);
                            if(MembStructType != NULL){
                            /*struct type variable need to set the pointer pointing to this
                            struct type*/
                                newMemVar.SetStructType(MembStructType);
                            }
                            newStructItem.AddMember(newMemVar);
                            VariableTab.AddItem(newMemVar);
                        }
                        
                        while(ChildNumber(DecList) != 1){
                        /*repeat what outside and just over this "while" */
                            DecList = GetChild(DecList, 3);
                            Dec = GetChild(DecList, 1);
                            CountDimension = 0;
                            VarDec = GetChild(Dec,1);
                            VarName = GetChild(VarDec, 1);
                            while(ChildNumber(VarDec) != 1){
                                VarDec = GetChild(VarDec, 1);
                                VarName = GetChild(VarDec, 1);
                                CountDimension += 1;
                            }
                            MemName = string(VarName->NodeUnit.LU.IDname);
                            if(ChildNumber(Dec) == 3){
                            /*Error type 15 ,initialize member var*/
                            /*Error but still add this memeber into varList as if Right
                            Firstly add Error, then Add member*/
                                SemanticError newError(Dec->lineno, MemName, 15);
                                ErrorList.AddError(newError);
                            }
                            if(CountDimension == 0){
                            /*this member variable is not an array*/
                                if(MembStructType == NULL){
                                /*this Member's type is not struct */ 
                                    MemberType = (StrMemberType == "int"?INT:FLOAT);
                                }
                                else
                                    MemberType = STRUCT;
                            }
                            else{
                            /*this member variable is an array*/
                                if(MembStructType == NULL)
                                    MemberType = (StrMemberType == "int"?INTARRAY:FLOATARRAY);
                                else
                                    MemberType = STRUCTARRAY; 

                            }    
                            /*before add, check it*/
                            if(newStructItem.GetMember(MemName) != NULL){
                            /*error type15, redefine of a struct member*/
                                SemanticError newError(Dec->lineno, MemName, 15);
                                ErrorList.AddError(newError);
                            }
                            else if(VariableTab.FindItem(MemName) != NULL){
                            /*error type 3*/
                                SemanticError newError(Dec->lineno, MemName, 3);
                                ErrorList.AddError(newError);
                            }
                            else{
                            /*no redefine*/
                                varItem newMemVar(MemName, MemberType, Dec->lineno, CountDimension);
                                if(MembStructType != NULL){
                                /*struct type variable need to set the pointer pointing to this
                                struct type*/
                                    newMemVar.SetStructType(MembStructType);
                                }
                                newStructItem.AddMember(newMemVar);
                                VariableTab.AddItem(newMemVar);
                            }
                        }
                        
                        DefList = GetChild(DefList, 2);
                    }
                    StructTab.AddItem(newStructItem);
                }

                else if(StructTab.FindItem(structName) != NULL && StructTab.FindItem(structName)->GetFstDefLine() == -1){
                /*Struct type had declared but not define*/
                }


                else{
                /*redefin of this struct type, error type16*/
                }
                
            }
        }
    }


    else{
    /*define or declare variable of function*/
    }




}

