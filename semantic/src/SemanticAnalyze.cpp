#include"SemanticAnalyze.h"
#include<iostream>
#include<assert.h>
/*class Analyze*/
void Analyze::TraverseTree(SyntaxTreeNode* node){
    if(node == NULL)
        return;
    if(node->type == Syntactic){
        
        string nodeName(node->NodeUnit.SU.name);
        
        if(nodeName == "ExtDef"){
            AnalyzeExtDef(node);
        }
        /*else if(nodeName == "Def")
            AnalyzeDef(node);
        else if(nodeName == "Exp")
            AnalyzeExp(node);*/
    }
    TraverseTree(node->child);
    TraverseTree(node->NextSibling);
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
            /*truely define a struct type*/
                
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
                    AnlzStruct(structSpecf, &newStructItem, UnDefinedStructType);
                }

                else if(StructTab.FindItem(structName) != NULL && StructTab.FindItem(structName)->GetFstDefLine() == -1){
                /*Struct type had declared but not define*/
                    structItem* DefItem = StructTab.FindItem(structName);
                    AnlzStruct(structSpecf, DefItem, DefinedStructType);            
                }


                else{
                /*redefin of this struct type, error type16*/
                SemanticError newError(structSpecf->lineno, structName, 16);
                ErrorList.AddError(newError);
                }
                
            }
        }
    }


    else{
    /*define or declare variable of function*/

    }




}



structItem* Analyze::AnlzStruct(SyntaxTreeNode* StructSpecf, structItem* OwnerStruct, int ifDef){
    string structName(OwnerStruct->GetName());
    
    SyntaxTreeNode* DefList(GetChild(StructSpecf, 4));
    SyntaxTreeNode* Def(NULL);
    SyntaxTreeNode* DefSpecf(NULL);
    string StrMemberType;
    TYPE MemberType(VOID);
    structItem* MembStructType(NULL);
    /*begin check member variable*/
    while(DefList->type != Empty){
    /*for every member def, first ensure Specifier,
    then dec*/
        Def = GetChild(DefList, 1);
        DefSpecf = GetChild(Def, 1);
        
        if(GetChild(DefSpecf, 1)->type == Lexical){
            /*base type member*/
            StrMemberType = string(GetChild(DefSpecf,1)->NodeUnit.LU.IDname);
        }
        else{
            /*struct type member*/
            SyntaxTreeNode* SubStructSpecf(GetChild(DefSpecf, 1));
            string SubStructName;
            if(ChildNumber(SubStructSpecf) == 2){
            /*nondefined struct or already-defined struct*/
                SubStructName = string(GetChild(GetChild(SubStructSpecf,2),1)->NodeUnit.LU.IDname);
                if((StructTab.FindItem(SubStructName) == NULL )||(StructTab.FindItem(SubStructName)->GetFstDefLine() == -1)){
                    /*no-defined struct,error type17 */
                    SemanticError newError(SubStructSpecf->lineno, SubStructName, 17);
                    ErrorList.AddError(newError);
                    DefList = GetChild(DefList, 2);
                    continue;
                }

                else{
                    MembStructType = StructTab.FindItem(SubStructName);
                }
            }
            else{
            /* defined whole struct type*/
                if(GetChild(SubStructSpecf,2)->type == Empty)
                    SubStructName = string("");
                else
                    SubStructName = string(GetChild(GetChild(SubStructSpecf,2),1)->NodeUnit.LU.IDname);
                if(StructTab.FindItem(SubStructName) == NULL || SubStructName == "")
                {/*first define this struct type*/
                    structItem newStruct(SubStructName, SubStructSpecf->lineno, SubStructSpecf->lineno);
                    MembStructType = AnlzStruct(SubStructSpecf, &newStruct, UnDefinedStructType); 
                }
                else if(StructTab.FindItem(SubStructName) != NULL && StructTab.FindItem(SubStructName)->GetFstDefLine()  == -1){
                /*declared before, need define now*/
                    MembStructType = AnlzStruct(SubStructSpecf, StructTab.FindItem(SubStructName), DefinedStructType);

                }
                else{
                /*redefined struct, error type16 */
                    SemanticError newError(SubStructSpecf->lineno, SubStructName, 16);
                    ErrorList.AddError(newError);
                    DefList = GetChild(DefList, 2);
                    continue;
                }

            
            }
        }

        SyntaxTreeNode* DecList(GetChild(Def, 2)); 
        SyntaxTreeNode* Dec(GetChild(DecList, 1));           
        int CountDimension(0);
        SyntaxTreeNode* VarDec(GetChild(Dec, 1));
        SyntaxTreeNode* VarName(GetChild(VarDec, 1));
        
        string MemName;

        do{
        /*check every MemDec */
            

            
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
                /*this Member's type is not struct or a redefined/undefined struct*/ 
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
            if(OwnerStruct->GetMember(MemName) != NULL){
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
                OwnerStruct->AddMember(newMemVar);
                VariableTab.AddItem(newMemVar);
            }
            
            if(ChildNumber(DecList) == 1)
                break;
            DecList = GetChild(DecList, 3);
            
        }
        while(1);
        //assert(0); 
        
        DefList = GetChild(DefList, 2);
    }
    if(ifDef == UnDefinedStructType){
        StructTab.AddItem(*OwnerStruct);
        return StructTab.GetBack();
    }
    else{
        OwnerStruct->SetFstDefLine(StructSpecf->lineno);
        return OwnerStruct;
    }
}
