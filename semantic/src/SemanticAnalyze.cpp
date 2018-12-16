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
                if(StructTab.FindItem(StructName) == NULL && VariableTab.FindItem(StructName) == NULL){
                /*struct type dec but not def*/
                    structItem newItem(StructName, structSpecf->lineno, -1);
                    StructTab.AddItem(newItem);
                }
                else{
                    if(VariableTab.FindItem(StructName) != NULL){
                    /*error type16*/
                        SemanticError *newErr = new SemanticError(structSpecf->lineno, StructName, 16);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                    }
                }
            
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
                    if(VariableTab.FindItem(structName) != NULL){
                    /*error type16, struct name same as a variable*/
                        SemanticError *newErr = new SemanticError(structSpecf->lineno, structName, 16);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                    }
                    structItem *newStructItem = new structItem(structName, structSpecf->lineno, structSpecf->lineno);
                    StructTab.AddItem(*newStructItem);
                    delete newStructItem;
                    newStructItem = StructTab.FindItem(structName);
                    AnlzStruct(structSpecf, newStructItem, DefinedStructType);
                    
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
                    int ArraySZ(10);
                    int *DimenSzArray = new int[ArraySZ];
                    while(ChildNumber(VarDec) != 1){
                        int temp(GetChild(VarDec, 3)->NodeUnit.LU.ival);
                        if(CountDimension >= ArraySZ){
                            ArraySZ += 10;
                            int *tempArray = new int[ArraySZ];
                            for (int i(0); i < CountDimension; i++)
                            {
                                tempArray[i] = DimenSzArray[i];
                            }
                            delete DimenSzArray;
                            DimenSzArray = tempArray;
                        }
                        DimenSzArray[CountDimension] = temp;
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
                    if(VariableTab.FindItem(StrVarName) != NULL || StructTab.FindItem(StrVarName) != NULL){
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
                        for (int i(0); i < CountDimension; i++){
                            newVar.setDimensionSize(i + 1, DimenSzArray[i]);
                        }
                        VariableTab.AddItem(newVar);
                    }
                    
                    if(ChildNumber(ExtDecList) == 1)
                        break;
                    ExtDecList = GetChild(ExtDecList, 3);
                    delete DimenSzArray;
                    DimenSzArray = NULL;
                } while (1);
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
            int ArraySZ(10);
            int *DimenSzArray = new int[ArraySZ];
            //not only get the dimension, but get size of each dimension
            while (ChildNumber(VarDec) != 1)
            {
                int temp(GetChild(VarDec, 3)->NodeUnit.LU.ival);
                if(CountDimension >= ArraySZ){
                    ArraySZ += 10;
                    int *tempArray = new int[ArraySZ];
                    for (int i(0); i < CountDimension; i++)
                    {
                        tempArray[i] = DimenSzArray[i];
                    }
                    delete DimenSzArray;
                    DimenSzArray = tempArray;
                }
                DimenSzArray[CountDimension] = temp;
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
                /*if(ChildNumber(DecList) == 1)
                    break;
                DecList = GetChild(DecList, 3);
                continue;*/

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
            else if(StructTab.FindItem(MemName) != NULL){
            /*error type 3*/
                SemanticError newError(Dec->lineno, MemName, 3);
                ErrorList.AddError(newError);    
            }
            //else{
            /*no redefine*/
            varItem newMemVar(MemName, MemberType, Dec->lineno, CountDimension);
            if(MembStructType != NULL){
            /*struct type variable need to set the pointer pointing to this
            struct type*/
                newMemVar.SetStructType(MembStructType);
            }
            for (int i(0); i < CountDimension; i++){
                newMemVar.setDimensionSize(i + 1, DimenSzArray[i]);
            }
            delete DimenSzArray;
            DimenSzArray = NULL;
            OwnerStruct->AddMember(newMemVar);
            VariableTab.AddItem(newMemVar);
            //}
            
            if(ChildNumber(DecList) == 1)
                break;
            DecList = GetChild(DecList, 3);
            
        }
        while(1);
        //assert(0); 
        
        DefList = GetChild(DefList, 2);
    }
    OwnerStruct->calculateSz();
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
                if(VariableTab.FindItem(SubStructName) != NULL){
                /*error type16*/
                    SemanticError *newErr = new SemanticError(SubStructSpecf->lineno, SubStructName, 16);
                    ErrorList.AddError(*newErr);
                    delete newErr;
                }
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
            if( *result == *NewFunc)
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
            result = FunctionTab.FindItem(FuncName);
        }
        else{
            if(result->NotDef()){
            /*function not def*/
                result->setDefLine(FuncNode->lineno);
                if(*result == *NewFunc){      
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
        AnalyzeCompSt(GetNextSibling(FuncNode), NewFunc);
        delete NewFunc;
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
            varItem* newArg= AnalyzeVarDec(GetChild(ParamDec, 2), StrVarType, structType, \
            (AnlzStyle == AnlzDec?LOCAL:GLOBAL));
            //if(newArg == NULL){
            /*redefined of variable*/
                
            //}
            //else{
                func.pushDefArg(*newArg);
                if(AnlzStyle == AnlzDef)
                    VariableTab.AddItem(*newArg);
                delete newArg;
            //}
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

varItem* Analyze::AnalyzeVarDec(SyntaxTreeNode* varDecNode, string &strType, \
structItem* structType, int VariableType){
    int CountDimension = 0;
    SyntaxTreeNode *VarDec = varDecNode;
    SyntaxTreeNode *VarName = GetChild(VarDec, 1);
    string strVarName;
    TYPE varType(VOID);

    int ArraySZ(10);
    int *DimenSzArray = new int[ArraySZ];
    while(ChildNumber(VarDec) != 1){
        int temp(GetChild(VarDec, 3)->NodeUnit.LU.ival);
        if(CountDimension >= ArraySZ){
            ArraySZ += 10;
            int *tempArray = new int[ArraySZ];
            for (int i(0); i < CountDimension; i++)
            {
                tempArray[i] = DimenSzArray[i];
            }
            delete DimenSzArray;
            DimenSzArray = tempArray;
        }
        DimenSzArray[CountDimension] = temp;
        VarDec = GetChild(VarDec, 1);
        VarName = GetChild(VarDec, 1);
        CountDimension += 1;
    }
    strVarName= string(VarName->NodeUnit.LU.IDname);
    if(VariableTab.FindItem(strVarName) != NULL && VariableType == GLOBAL){
    /*error type3*/
        SemanticError newError(varDecNode->lineno, strVarName, 3);
        ErrorList.AddError(newError);
        //return NULL;
    }
    if(StructTab.FindItem(strVarName) != NULL && VariableType == GLOBAL){
    /*error type3*/
        SemanticError newError(varDecNode->lineno, strVarName, 3);
        ErrorList.AddError(newError);   
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
    for (int i(0); i < CountDimension; i++){
        newVar->setDimensionSize(i + 1, DimenSzArray[i]);
    }
    delete DimenSzArray;
    return newVar;
}

void Analyze::AnalyzeCompSt(SyntaxTreeNode* CompStNode, funItem* FatherFunc){
    SyntaxTreeNode* DefListNode(GetChild(CompStNode, 2));
    SyntaxTreeNode* StmtListNode(GetChild(CompStNode, 3));

    while(GetNodeType(DefListNode) != Empty){
        AnalyzeDef(GetChild(DefListNode,1));
        DefListNode = GetChild(DefListNode, 2);
    }

    while(GetNodeType(StmtListNode) != Empty){
        AnalyzeStmt(GetChild(StmtListNode, 1), FatherFunc);
        StmtListNode = GetChild(StmtListNode, 2);
    }
}

void Analyze::AnalyzeStmt(SyntaxTreeNode* StmtNode, funItem* FatherFunc){
    int ChildrenNum = ChildNumber(StmtNode);
    if(ChildrenNum == 1){
        AnalyzeCompSt(GetChild(StmtNode, 1), FatherFunc);
    }
    else if(ChildrenNum == 2){
        AnalyzeExp(GetChild(StmtNode, 1));
    }
    else if(ChildrenNum == 3){
        varItem * tempVar = new varItem(AnalyzeExp(GetChild(StmtNode, 2)));
        if(tempVar->GetType() != FatherFunc->GetRetType()){
        /*error type8*/
            SemanticError *newError = new SemanticError(StmtNode->lineno, FatherFunc->GetName(), 8);
            ErrorList.AddError(*newError);
            delete newError;
        }
        else{
            if(tempVar->GetType() == STRUCT){
                if(tempVar->GetStructType() != FatherFunc->GetRetStruct())
                    if(*(tempVar->GetStructType()) != *(FatherFunc->GetRetStruct())){
                    /*error type 8*/
                        SemanticError *newErr = new SemanticError(StmtNode->lineno, FatherFunc->GetName(), 8);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                    }
            }
        }
    }
    else {
        SyntaxTreeNode* fstChild(GetChild(StmtNode, 1));
        if(fstChild->NodeUnit.LU.Lextype == LIF){
            varItem *ExpVar = new varItem(AnalyzeExp(GetChild(StmtNode, 3)));
            if(ExpVar->GetType() != INT){
                /*error type 20*/
                    SemanticError *newErr = new SemanticError(fstChild->lineno, "", 20);
                    ErrorList.AddError(*newErr);
                    delete newErr;
            }
            delete ExpVar;
            if(ChildNumber(StmtNode) == 5){
            /*if-without-else*/

                AnalyzeStmt(GetChild(StmtNode, 5), FatherFunc);
            }
            else{
            /*if-else*/
                AnalyzeStmt(GetChild(StmtNode, 5), FatherFunc);
                AnalyzeStmt(GetChild(StmtNode, 7), FatherFunc);
            }
        }
        else{
        /*while stmt*/
            varItem *ExpVar = new varItem(AnalyzeExp(GetChild(StmtNode, 3)));
            if(ExpVar->GetType() != INT){
                /*error type 20*/
                    SemanticError *newErr = new SemanticError(fstChild->lineno, "", 20);
                    ErrorList.AddError(*newErr);
                    delete newErr;
            }
            delete ExpVar;
            AnalyzeStmt(GetChild(StmtNode, 5), FatherFunc);
        }
    }
}


void Analyze::AnalyzeDef(SyntaxTreeNode* DefNode){
    SyntaxTreeNode* SpecfNode(GetChild(DefNode, 1));
    SyntaxTreeNode* DecListNode(GetChild(DefNode, 2));
    string StrVarType;
    structItem* VarStructType(NULL);
    if(AnlzSpecf(SpecfNode, StrVarType, VarStructType)){
        SyntaxTreeNode *DecNode(GetChild(DecListNode, 1));
        varItem* newVariable(NULL);
        while(1){
            newVariable = AnalyzeVarDec(GetChild(DecNode, 1), StrVarType, VarStructType, GLOBAL);
            if(ChildNumber(DecNode) == 1){
            /*variable not initialized*/
                if(newVariable != NULL){
                    VariableTab.AddItem(*newVariable);
                    delete newVariable;
                }
            }
            else{
            /*varaible initialized*/
                if(newVariable != NULL){
                    /*TYPE ExpType(VOID);
                    int ExpDimension(0);
                    structItem* ExpStructType(NULL);*/
                    varItem *tempVar = new varItem(AnalyzeExp(GetChild(DecNode, 3)));
                    /*if(tempVar->GetType() != newVariable->GetType()){
                    /*error type 5
                        SemanticError *newErr = new SemanticError(DefNode->lineno, "", 5);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                    }
                    else{
                        if(tempVar->GetType() == STRUCT){
                            if(tempVar->GetStructType() != newVariable->GetStructType())
                                if(*(tempVar->GetStructType()) != *(newVariable->GetStructType())){
                                    SemanticError *newErr = new SemanticError(DefNode->lineno, "", 5);
                                    ErrorList.AddError(*newErr);
                                    delete newErr;
                                }
                        }
                    }*/
                    if(*tempVar!= *newVariable){
                    /*error type 5*/
                        SemanticError *newErr = new SemanticError(DefNode->lineno, "", 5);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                    }
                    else{
                        
                    }
                    VariableTab.AddItem(*newVariable);
                    delete newVariable;

                }
                
            }
            if(ChildNumber(DecListNode) == 1)
                break;
            DecListNode = GetChild(DecListNode, 3);
            DecNode = GetChild(DecListNode, 1);
        }
    }
}

varItem Analyze::AnalyzeExp(SyntaxTreeNode* ExpNode){
    if(ChildNumber(ExpNode) == 1){
        SyntaxTreeNode* ExpChild = GetChild(ExpNode, 1);
        if(ExpChild->NodeUnit.LU.Lextype == LID){
            string strID(ExpChild->NodeUnit.LU.IDname);
            if(VariableTab.FindItem(strID) == NULL){
            /*undefined variable, error type1*/
                SemanticError newError(ExpNode->lineno, strID, 1);
                ErrorList.AddError(newError);
                return varItem("$error1", VOID, -1, 0);
            }
            else{
                return *(VariableTab.FindItem(strID));
            }
        }
        else if(ExpChild->NodeUnit.LU.Lextype == LINT){
            string ConstInt("$ConstInt");
            varItem constVariable(ConstInt, INT, ExpChild->lineno, 0);
            return constVariable;
        }
        else{
            string ConstFloat("$ConstFloat");
            varItem constVariable(ConstFloat, FLOAT, ExpChild->lineno, 0);
            return constVariable;
        }
    }

    else if(ChildNumber(ExpNode) == 2){
    /*three productions*/
        SyntaxTreeNode *SubExpNode = GetChild(ExpNode, 2);
        SyntaxTreeNode *OptNode = GetChild(ExpNode, 1);
        varItem SubExp = AnalyzeExp(SubExpNode);
        if(OptNode->NodeUnit.LU.Lextype == LNOT){
            if(SubExp.GetType() != INT){
            /*error type7, operands mismatch*/
                SemanticError newError(SubExpNode->lineno, SubExp.GetName(), 7);
                ErrorList.AddError(newError);
                return varItem("$const", VOID, -1, 0);
            }
            else{
                varItem retVal("$const", SubExp.GetType(), SubExp.GetLineNo(), SubExp.GetDimension());
                retVal.SetStructType(SubExp.GetStructType());
                return retVal;
            }
        }
        else{
            if(SubExp.GetType() != INT && SubExp.GetType() != FLOAT){
            /*error type7, operands mismatch*/
                SemanticError newError(SubExpNode->lineno, SubExp.GetName(), 7);
                ErrorList.AddError(newError);
                return varItem("$const", VOID, -1, 0);
            }
            else{
                varItem retVal("$const", SubExp.GetType(), SubExp.GetLineNo(), SubExp.GetDimension());
                retVal.SetStructType(SubExp.GetStructType());
                return retVal;
            }
        }
    }
    else{
        SyntaxTreeNode *fstChildNode = GetChild(ExpNode, 1);
        if(fstChildNode->type == Lexical && fstChildNode->NodeUnit.LU.Lextype == LID){
        /*call function*/
            string funcName(fstChildNode->NodeUnit.LU.IDname);
            funItem* calledFunc(FunctionTab.FindItem(funcName));
            if(calledFunc != NULL){
            /*find func, then check ArgList*/
                if(ChildNumber(ExpNode) == 3){
                /*Exp has no Arg*/
                    if(calledFunc->GetArgListSize() == 0){
                        varItem retVal("$const", calledFunc->GetRetType(), ExpNode->lineno, 0);
                        retVal.SetStructType(calledFunc->GetRetStruct());
                        return retVal;
                    }
                    else{
                    /*error type9*/
                        SemanticError newErr(ExpNode->lineno, funcName, 9);
                        ErrorList.AddError(newErr);
                        return varItem("$const", VOID, -1, 0);
                    }
                }

                else{
                /*Exp has Arg*/
                    vector<varItem> *tempArgList = AnalyzeArgs(GetChild(ExpNode,3));
                    assert(tempArgList->size() != 0);
                    if(*tempArgList == calledFunc->GetArgList()){
                        varItem retVal("$const", calledFunc->GetRetType(), ExpNode->lineno, 0);
                        retVal.SetStructType(calledFunc->GetRetStruct());
                        delete tempArgList;
                        return retVal;
                    }
                    else{
                    /*error type9*/
                        SemanticError *newErr = new SemanticError(ExpNode->lineno, funcName, 9);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                        delete tempArgList;
                        return varItem("$const", VOID, -1, 0);
                    }
                }
            }
            else{
            /*not find func*/
                if(VariableTab.FindItem(funcName) != NULL){
                /*error type11*/
                    SemanticError newErr(fstChildNode->lineno, funcName, 11);
                    ErrorList.AddError(newErr);
                    return varItem("$VarAsFunc", VOID, -1, 0);
                }
                else{
                /*error type2*/
                    SemanticError newErr(fstChildNode->lineno, funcName, 2);
                    ErrorList.AddError(newErr);
                    return varItem("$unDefFunc", VOID, -1 , 0);
                }
            }  
        }

        else if(fstChildNode->type == Lexical && fstChildNode->NodeUnit.LU.Lextype == LLP){
        /*exp like "(a + b)"*/
            SyntaxTreeNode* subExpNode(GetChild(ExpNode, 2));
            return AnalyzeExp(subExpNode);
        }
        else{
            SyntaxTreeNode* SecChildNode(GetChild(ExpNode, 2));
            if(SecChildNode->NodeUnit.LU.Lextype == LDOT){
            /*visit struct member*/
                varItem *tempExpVar = new varItem(AnalyzeExp(fstChildNode));
                if(tempExpVar->GetType() != STRUCT){
                /*error type13, illegal use of '.'*/
                    SemanticError newErr(fstChildNode->lineno, tempExpVar->GetName(), 13);
                    ErrorList.AddError(newErr);
                    delete tempExpVar;
                    return varItem("$error13", VOID, -1, 0);
                }
                else{
                    SyntaxTreeNode* IDNode = GetChild(ExpNode, 3);
                    string IDname(IDNode->NodeUnit.LU.IDname);
                    varItem* structMember(tempExpVar->GetStructType()->GetMember(IDname));
                    if(structMember == NULL){
                    /*error type14, using non-exist member*/
                        SemanticError newErr(IDNode->lineno, IDname, 14);
                        ErrorList.AddError(newErr);
                        delete tempExpVar;
                        return varItem("$wrongMemb", VOID, -1, 0);
                    }
                    else{
                        delete tempExpVar;
                        return *structMember;
                    }
                }
            }

            else if(SecChildNode->NodeUnit.LU.Lextype == LLB){
            /*array visit*/
                SyntaxTreeNode *SubExpNode1(GetChild(ExpNode, 1));
                SyntaxTreeNode *SubExpNode2(GetChild(ExpNode, 3));
                varItem Exp1(AnalyzeExp(SubExpNode1));
                varItem Exp2(AnalyzeExp(SubExpNode2));
                if(Exp2.GetType() != INT){
                /*error type12*/
                    SemanticError *newErr = new SemanticError(SubExpNode2->lineno, Exp2.GetName(), 12);
                    ErrorList.AddError(*newErr);
                    delete newErr;
                    return varItem("$error12", VOID, -1, 0);
                }
                if(Exp1.GetDimension() <= 0){
                /*error type 10, using '[]' on non-array type*/
                    SemanticError *newErr = new SemanticError(SubExpNode1->lineno, Exp1.GetName(), 10);
                    ErrorList.AddError(*newErr);
                    delete newErr;
                    return varItem("$error10", VOID, -1, 0);
                }
                TYPE retType(VOID);
                if(Exp1.GetDimension() - 1 == 0){
                    if(Exp1.GetType() == INTARRAY)
                        retType = INT;
                    else if(Exp1.GetType() == FLOATARRAY)
                        retType = FLOAT;
                    else 
                        retType = STRUCT;
                    varItem retval(Exp1.GetName(), retType, Exp1.GetLineNo(), Exp1.GetDimension() - 1);
                    retval.SetStructType(Exp1.GetStructType());
                    //retval.setDimensionSize(Exp1);
                    //std::cout << retval.GetDimension() << " " << retval.getBaseTypeSize() << std::endl;
                    return retval;
                }
                else{
                    varItem retval(Exp1.GetName(), Exp1.GetType(), Exp1.GetLineNo(), Exp1.GetDimension() - 1);
                    retval.SetStructType(Exp1.GetStructType());
                    retval.setDimensionSize(Exp1);
                    //std::cout << retval.GetDimension() << " " << retval.getBaseTypeSize() << std::endl;
                    return retval;
                }     
            }

            else{
            /*two element operators*/
                SyntaxTreeNode* opNode(GetChild(ExpNode, 2));
                SyntaxTreeNode* SubExpNode1(GetChild(ExpNode, 1));
                SyntaxTreeNode* SubExpNode2(GetChild(ExpNode, 3));
                varItem Exp1(AnalyzeExp(SubExpNode1));
                varItem Exp2(AnalyzeExp(SubExpNode2));
                if(opNode->NodeUnit.LU.Lextype == LASSIGNOP){
                /*Assign operation*/
                    if(VariableTab.FindItem(Exp1.GetName()) == NULL){
                    /*check left value failed, error type6*/
                        SemanticError *newErr = new SemanticError(SubExpNode1->lineno, Exp1.GetName(), 6);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                        return varItem("$error6", VOID, -1, 0);
                    }
                    else{
                    /*type check for both sides*/
                        if(Exp1 == Exp2){
                            return Exp1;
                        }
                        else{
                        /*error type5*/
                            SemanticError *newErr = new SemanticError(ExpNode->lineno, Exp1.GetName(), 5);
                            ErrorList.AddError(*newErr);
                            delete newErr;
                            return varItem("$error5", VOID, -1, 0);
                        }
                    }
                }

                else if(opNode->NodeUnit.LU.Lextype == LAND || \
                opNode->NodeUnit.LU.Lextype == LOR ){
                /*logical arithmetic*/
                    if(Exp1.GetType() != INT || Exp2.GetType() != INT){
                    /*error type7*/
                        SemanticError *newErr = new SemanticError(SubExpNode1->lineno, Exp1.GetName(), 7);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                        return varItem("$error7", VOID, -1, 0);
                    }
                    else{
                        return varItem("$constInt", INT, SubExpNode1->lineno, 0);
                    }
                }
                else if(opNode->NodeUnit.LU.Lextype == LRELOP){
                    if(Exp1.GetType() == INT && Exp2.GetType() == INT){
                        return varItem("$constInt", INT, SubExpNode1->lineno, 0);
                    }
                    else if(Exp1.GetType() == FLOAT && Exp2.GetType() == FLOAT){
                        return varItem("$constInt", INT, SubExpNode1->lineno, 0);
                    }
                    else {
                        SemanticError *newErr = new SemanticError(SubExpNode1->lineno, Exp1.GetName(), 7);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                        return varItem("$error7", VOID, -1, 0);
                    }
                }

                else{
                /*plus add subtract and divide*/
                    if(Exp1.GetType() == INT && Exp1.GetType() == Exp2.GetType()){
                        return varItem("$constInt", INT, SubExpNode1->lineno, 0);
                    }
                    else if(Exp1.GetType() == FLOAT && Exp1.GetType() == Exp2.GetType()){
                        return varItem("$constFloat", FLOAT, SubExpNode1->lineno, 0);
                    }
                    else{
                    /*error type 7*/
                        SemanticError *newErr = new SemanticError(ExpNode->lineno, Exp1.GetName(), 7);
                        ErrorList.AddError(*newErr);
                        delete newErr;
                        return varItem("$error7", VOID, -1, 0);
                    }
                }


            }
        }
    }
}

vector<varItem>* Analyze::AnalyzeArgs(SyntaxTreeNode* ArgsNode){
    SyntaxTreeNode* argsNode(ArgsNode);
    SyntaxTreeNode* ExpNode(GetChild(argsNode, 1));
    vector<varItem> *retVec = new vector<varItem>;
    while(1){
        varItem *retVar = new varItem(AnalyzeExp(ExpNode));
        retVec->push_back(*retVar);
        delete retVar;
        if(ChildNumber(argsNode) == 1){
            break;
        }
        argsNode = GetChild(argsNode, 3);
        ExpNode = GetChild(argsNode, 1);
    }
    return retVec;
}

void Analyze::CheckFunTab(){
    list<funItem>::iterator it = FunctionTab.table.begin();
    while(it != FunctionTab.table.end()){
        if(it->NotDef()){
        /*error type 18*/
            SemanticError *newErr  = new SemanticError(it->GetDecLine(), it->GetName(), 18);
            ErrorList.AddError(*newErr);
            delete newErr;
        }
        it++;
    }
    string *str_read = new string("read");
    string *str_write = new string("write");
    varItem* write_arg = new varItem("output", INT, 0, 0);
    funItem* read = new funItem(*str_read, INT, 0, 0);
    funItem* write = new funItem(*str_write, INT, 0, 0);
    write->pushDefArg(*write_arg);
    FunctionTab.AddItem(*read);
    FunctionTab.AddItem(*write);
    delete write;
    delete read;
    delete write_arg;
    delete str_read;
    delete str_write;


}



