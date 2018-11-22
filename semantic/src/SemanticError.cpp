#include "SemanticError.h"
#include<sstream>
#include<iostream>
using std::cout;
using std::endl;
/************************************
 class SemanticError*/

string SemanticError::errorType[19] = {
    "Undefined variable \""/*variable*/,      //error type1
    "Undefined function \""/*variable*/,      //error type2
    "Redefined variable \""/*variable*/,      //error type3
    "Redefined function \""/*variable*/,      //error type4
    "Type mismatched for assignment.",      //error type5
    "The left-hand side of an assignment must be a variable.",  //error type6
    "Type mismatched for operands.",     //error type7
    "Type mismatched for return.",       //error type8
    /*"Function " [funcname]*/"\" is not applicable for arguments.",//error type9
    /*variable*/ "\" is not an array.", //error type10
    /*variable*/ "\" is not a function.",   //error type11
    /*variable*/"\" is not an integer.",    //error type12
    "Illegal us of \".\"",   //error type13
    "Non-existent field \"" /* "varaible" */, //error type14
    "Redefined field or initialize field \"" /* "variable" */, //error type15
    "Duplicated name \"" /* "struct name"*/, //error type16
    "Undefined structure \"" /* "struct name" */,  //error type17
    "Declared but Undefined function \""/* "func name" */,  //errortype 18
    "Inconsistent declaration of function \"" /* "func name" */     //error type19

};

void SemanticError::SpliceErrorInfo(int lineNo, string ObjectName, int errorNo){
   std::stringstream ss;
   string strLineNo;
   string strErrorNo;
   ss << lineNo;
   ss >>strLineNo;
   ss.clear();
   ss<<errorNo;
   ss>>strErrorNo;
   switch(errorNo){
        case 1:
        errorInfo ="Error Type " + strErrorNo +" at Line " + strLineNo + ":"\
        + errorType[0] + ObjectName;
        errorInfo.append("\".");
        break;
        
        case 2:
        errorInfo ="Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        +  errorType[1] + ObjectName;
        errorInfo.append("\".");
        break;

        case 3:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        + errorType[2] + ObjectName;
        errorInfo.append("\".");
        break;

        case 4:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        + errorType[3] + ObjectName;
        errorInfo.append("\".");
        break;

        case 5:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" + errorType[4];
        break;

        case 6:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" + errorType[5];
        break;

        case 7:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" +errorType[6];
        break;

        case 8:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" +errorType[7];
        break;

        case 9:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        +ObjectName + errorType[8];
        break;

        case 10:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        +"\"" + ObjectName + errorType[9];
        break;

        case 11:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
        +"\"" + ObjectName + errorType[10];
        break;

        case 12:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            + "\"" + ObjectName + errorType[11];
        break;

        case 13:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            +errorType[12];
        break;

        case 14:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            + errorType[13] + ObjectName + "\"";
        break;

        case 15:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            +errorType[14] + ObjectName + "\"";
        break;

        case 16:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            +errorType[15] + ObjectName + "\"";
        break;

        case 17:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            + errorType[16] + ObjectName + "\"";
        break;

        case 18:
        errorInfo = "Error Type " + strErrorNo +" at Line " + strLineNo + ":" \
            + errorType[17] + ObjectName + "\"";
        break;

        case 19:
        errorInfo = "Error Type " + strErrorNo +" at Line  " + strLineNo + ":" \
            + errorType[18] + ObjectName + "\"";
        break;
    }
}




/*class SemanticErrorList*/
void SemanticErrorList::PrintErrorList(){
    for(int i(0); i < errorList.size(); i++){
        cout << errorList[i].errorInfo <<endl;
    }
}


