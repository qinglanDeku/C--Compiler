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
    }
    TraverseTree(node->child);
    TraverseTree(node->NextSibling);
}


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
        string SecondNodeName(GetChild(ExtDefNode, 2)->NodeUnit.SU.name);
        if(SecondNodeName == "ExtDecList"){
        /*define variable*/
            
            SyntaxTreeNode *DefSpecf(GetChild(ExtDefNode, 1));
            string StrVarType;
            TYPE varType;
            structItem* structType(NULL);
            if(AnlzSpecf(DefSpecf, StrVarType, structType)){
                
                SyntaxTreeNode* ExtDecList(GetChild(ExtDefNode, 2));        
                int CountDimension(0);
                SyntaxTreeNode* VarDec(GetChild(ExtDecList, 1));
                SyntaxTreeNode* VarName(GetChild(VarDec, 1));
                
                string StrVarName;

                do{
                /*check every MemDec */
                    

                    
                    VarDec = GetChild(ExtDecList, 1);
                    
                    CountDimension = 0;
                    VarName = GetChild(VarDec, 1);
                    
                    while(ChildNumber(VarDec) != 1){
                        
                        VarDec = GetChild(VarDec, 1);
                        // assert(ChildNumber(VarDec) != 1);
                        VarName = GetChild(VarDec, 1);
                        CountDimension += 1;
                        
                    }
                    
                    StrVarName = string(VarName->NodeUnit.LU.IDname);
                    
                    if(CountDimension == 0){
                    /*this member variable is not an array*/
                        if(structType == NULL){
                        /*this Member's type is not struct or a redefined/undefined struct*/ 
                            varType = (StrVarType == "int"?INT:FLOAT);
                        }
                        else
                            varType = STRUCT;
                    }
                    else{
                    /*this member variable is an array*/
                        if(structType == NULL)
                            varType = (StrVarType == "int"?INTARRAY:FLOATARRAY);
                        else
                            varType = STRUCTARRAY; 
                    }
                    
                    /*before add, check it*/
                    if(VariableTab.FindItem(StrVarName) != NULL){
                    /*error type 3*/
                        SemanticError newError(VarDec->lineno, StrVarName, 3);
                        ErrorList.AddError(newError);
                    }
                    else{
                    /*no redefine*/
                        varItem newVar(StrVarName, varType, VarDec->lineno, CountDimension);
                        if(structType != NULL){
                        /*struct type variable need to set the pointer pointing to this
                        struct type*/
                            newVar.SetStructType(structType);
                        }
                        VariableTab.AddItem(newVar);
                    }
                    
                    if(ChildNumber(ExtDecList) == 1)
                        break;
                    ExtDecList = GetChild(ExtDecList, 3);
                    
                }
                while(1);
                
            }
        }
        else{
            /*define or declare function*/
            SyntaxTreeNode* RetvalSpecf(GetChild(ExtDefNode, 1));
            string StrRetType;
            TYPE retType;
            structItem* structType(NULL);
            if(!AnlzSpecf(RetvalSpecf, StrRetType, structType))
                retType = VOID;//rettype is wrong
            else{
                setRetType(StrRetType, retType);
            }
            AnlzFunc(GetChild(ExtDefNode, 2), retType, structType);
        }

        
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
        
        if(!AnlzSpecf(DefSpecf, StrMemberType, MembStructType)){
            DefList = GetChild(DefList, 2);
            continue;
        }
        else
            ;
            
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
                if(ChildNumber(DecList) == 1)
                    break;
                DecList = GetChild(DecList, 3);
                continue;

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

bool Analyze::AnlzSpecf(SyntaxTreeNode* DefSpecf, string &StrType, structItem* &StructType){
    if(GetChild(DefSpecf, 1)->type == Lexical){
        /*base type member*/
        StrType = string(GetChild(DefSpecf,1)->NodeUnit.LU.IDname);
    }
    else{
        /*struct type member*/
        SyntaxTreeNode* SubStructSpecf(GetChild(DefSpecf, 1));
        string SubStructName;
        StrType = "struct";
        if(ChildNumber(SubStructSpecf) == 2){
        /*nondefined struct or already-defined struct*/
            SubStructName = string(GetChild(GetChild(SubStructSpecf,2),1)->NodeUnit.LU.IDname);
            if((StructTab.FindItem(SubStructName) == NULL )||(StructTab.FindItem(SubStructName)->GetFstDefLine() == -1)){
                /*no-defined struct,error type17 */
                SemanticError newError(SubStructSpecf->lineno, SubStructName, 17);
                ErrorList.AddError(newError);
                return false;
            }

            else{
                StructType = StructTab.FindItem(SubStructName);
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
                StructType = AnlzStruct(SubStructSpecf, &newStruct, UnDefinedStructType); 
            }
            else if(StructTab.FindItem(SubStructName) != NULL && StructTab.FindItem(SubStructName)->GetFstDefLine()  == -1){
            /*declared before, need define now*/
                StructType = AnlzStruct(SubStructSpecf, StructTab.FindItem(SubStructName), DefinedStructType);

            }
            else{
            /*redefined struct, error type16 */
                SemanticError newError(SubStructSpecf->lineno, SubStructName, 16);
                ErrorList.AddError(newError);
                return false;
            }

        
        }
    }
    return true;
}

void Analyze::AnlzFunc(SyntaxTreeNode* FuncNode, TYPE retType, structItem* structType){
    SyntaxTreeNode* FuncNameNode(GetChild(FuncNode, 1));
    string FuncName(FuncNameNode->NodeUnit.LU.IDname);
    if(GetNodeType(GetNextSibling(FuncNode)) == Lexical){
    /*this is a func declaration*/

        funItem* NewFunc = new funItem(FuncName, retType, FuncNode->lineno);
        NewFunc->SetRetStruct(structType);
        if(ChildNumber(FuncNode) == 3);//have no arglist do nothing
        else
            AnlzFuncArgList(GetChild(FuncNode, 3), *NewFunc, AnlzDec);
        
        funItem* result = FunctionTab.FindItem(FuncName);
        if( result == NULL){
        /*first declare*/
            FunctionTab.AddItem(*NewFunc);
        }
        else{
        /*have been defined or declared*/
            if( *result== *NewFunc)
            /*just declare, do nothing*/;
            else{
            /*error type 19, inconsistent function declaration or definition*/
                SemanticError newError(FuncNode->lineno, FuncName, 19);
                ErrorList.AddError(newError);
            }
        }
        delete NewFunc;
    }

    else{
    /*this is a func defination*/ 
        funItem* NewFunc = new funItem(FuncName, retType, FuncNode->lineno, FuncNode->lineno);
        NewFunc->SetRetStruct(structType);
        if(ChildNumber(FuncNode) == 3)
        /*have no arglist, do nothing*/;
        else
            AnlzFuncArgList(GetChild(FuncNode, 3), *NewFunc, AnlzDef);
        
        funItem* result = FunctionTab.FindItem(FuncName);
        if(result == NULL){
        /*have not been defined or declared*/
            FunctionTab.AddItem(*NewFunc);
        }
        else{
            if(result->NotDef()){
            /*function not def*/
                if(*result == *NewFunc){
                    result->setDefLine(FuncNode->lineno);
                    result->CopyDefArgList(*NewFunc);    
                }
                else{
                /*error type 19, confilct between function definition and declaration*/
                    SemanticError NewError(FuncNode->lineno, FuncName, 19);
                    ErrorList.AddError(NewError);
                } 
            }

            else{
            /*function redefine, error type 4*/
                SemanticError NewError(FuncNode->lineno, FuncName, 4);
                ErrorList.AddError(NewError);
            }
        }
        AnalyzeCompSt(GetNextSibling(FuncNode));
    }
}

void Analyze::AnlzFuncArgList(SyntaxTreeNode* ArgListNode, funItem& func, char AnlzStyle){
    SyntaxTreeNode* varList(ArgListNode);
    SyntaxTreeNode* ParamDec(GetChild(varList, 1));
    string StrVarType;
    structItem* structType(NULL);
    while(1){
        if(!AnlzSpecf(GetChild(ParamDec, 1), StrVarType,structType)){
        /*specifier illegal*/
            if(ChildNumber(varList) == 1)
                break;
            else{
                varList = GetChild(varList, 3);
                ParamDec = GetChild(varList, 1);
                continue;
            }
        }
        else{
            varItem* newArg= AnalyzeVarDec(GetChild(ParamDec, 2), StrVarType, structType);
            if(newArg == NULL){
            /*redefined of variable*/
            }
            else{
                func.pushDefArg(*newArg);
                if(AnlzStyle == AnlzDef)
                    VariableTab.AddItem(*newArg);
                delete newArg;
            }
            if(ChildNumber(varList) == 1)
                break;
            else{
                varList = GetChild(varList, 3);
                ParamDec = GetChild(varList, 1);
                continue;
            }
            
        }
    }
}

varItem* Analyze::AnalyzeVarDec(SyntaxTreeNode* varDecNode, string &strType, structItem* structType){
    int CountDimension = 0;
    SyntaxTreeNode *VarDec = varDecNode;
    SyntaxTreeNode *VarName = GetChild(VarDec, 1);
    string strVarName;
    TYPE varType(VOID);

    
    while(ChildNumber(VarDec) != 1){
        VarDec = GetChild(VarDec, 1);
        VarName = GetChild(VarDec, 1);
        CountDimension += 1;
    }
    strVarName= string(VarName->NodeUnit.LU.IDname);
    if(VariableTab.FindItem(strVarName) != NULL){
    /*error type3*/
        SemanticError newError(varDecNode->lineno, strVarName, 3);
        ErrorList.AddError(newError);
        return NULL;
    }

    if(strType == "int"){
        if(CountDimension > 0)
            varType = INTARRAY;
        else
            varType = INT;
    }
    else if(strType == "float"){
        if(CountDimension > 0)
            varType = FLOATARRAY;
        else
            varType = FLOAT;
    }
    else{
        if(CountDimension > 0)
            varType = STRUCTARRAY;
        else 
            varType = STRUCT;
    }
    
    varItem* newVar = new varItem(strVarName, varType, varDecNode->lineno, CountDimension);
    newVar->SetStructType(structType);
    return newVar;

}

void Analyze::AnalyzeCompSt(SyntaxTreeNode* CompStNode){
    SyntaxTreeNode* DefListNode(GetChild(CompStNode, 2));
    SyntaxTreeNode* StmtListNode(GetChild(CompStNode, 3));

    while(GetNodeType(DefListNode) != Empty){
        AnalyzeDef(GetChild(DefListNode,1));
        DefListNode = GetChild(DefListNode, 2);
    }

    while(GetNodeType(StmtListNode) != Empty){
        //AnalyzeStmt(GetChild(StmtListNode, 1));
        StmtListNode = GetChild(StmtListNode, 2);
    }
}


void Analyze::AnalyzeDef(SyntaxTreeNode* DefNode){
    SyntaxTreeNode* SpecfNode(GetChild(DefNode, 1));
    SyntaxTreeNode* DecListNode(GetChild(DefNode, 2));
    string StrVarType;
    structItem* VarStructType(NULL);
    if(AnlzSpecf(SpecfNode, StrVarType, VarStructType)){
        SyntaxTreeNode *Dec(GetChild(DecListNode, 1));
        varItem* newVariable(NULL);
        while(1){
            newVariable = AnalyzeVarDec(GetChild(Dec, 1), StrVarType, VarStructType);
            if(ChildNumber(Dec) == 1){
            /*variable not initialized*/
                if(newVariable != NULL){
                    VariableTab.AddItem(*newVariable);
                    delete newVariable;
                }
            }
            else{
            /*varaible initialized*/
                if(newVariable != NULL){
                    TYPE ExpType(VOID);
                    int ExpDimension(0);
                    structItem* ExpStructType(NULL);
                    AnalyzeExp(GetChild(Dec, 3), ExpType, ExpDimension, ExpStructType);

                }
                
            }
        }
    }
}

void Analyze::AnalyzeExp(SyntaxTreeNode* ExpNode, TYPE &ExpType, int &ExpDimension,\
structItem* &ExpStructType){
    if(ChildNumber(ExpNode) == 1){
        SyntaxTreeNode* ExpChild = GetChild(ExpNode, 1);
        if(ExpChild->NodeUnit.LU.Lextype == LID){
            string strID(ExpChild->NodeUnit.LU.IDname);
            if(VariableTab.FindItem(strID) == NULL){
            /*undefined variable, error type1*/
            }
        }
    }
}



