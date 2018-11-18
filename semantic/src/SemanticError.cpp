#include "SemanticError.h"
#include<sstream>
/************************************
 class SemanticErrorInfo*/

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
    "Redefined field \"" /* "variable" */, //error type15
    "Duplicated name \"" /* "struct name"*/, //error type16
    "Undefined structure \"" /* "struct name" */,  //error type17
    "Inconsistent declaration of function \"" /* "func name" */,  //errortype 18
    "Declared but Undefined function \""/* "func name" */     //error type19

};

void SemanticError::SpliceErrorInfo(int lineNo, string ObjectName, int errorNo){
   std::stringstream ss;
   string strLineNo;
   ss << lineNo;
   ss >>strLineNo;
   switch(errorNo){
        case 1:
        errorInfo ="Error Type at Line" + strLineNo + ":"\
        + errorType[0] + ObjectName;
        errorInfo.append("\".");
        break;
        
        case 2:
        errorInfo ="Error Type at line" + strLineNo + ":" \
        +  errorType[1] + ObjectName;
        errorInfo.append("\".");
        break;

        case 3:
        errorInfo = "Error Type at line" + strLineNo + ":" \
        + errorType[2] + ObjectName;
        errorInfo.append("\".");
        break;

        case 4:
        errorInfo = "Error Type at line" + strLineNo + ":" \
        + errorType[3] + ObjectName;
        errorInfo.append("\".");
        break;

        case 5:
        errorInfo = "Error Type at line" + strLineNo + ":" + errorType[4];
        break;

        case 6:
        errorInfo = "Error Type at line" + strLineNo + ":" + errorType[5];
        break;

        case 7:
        errorInfo = "Error Type at line" + strLineNo + ":" +errorType[6];
        break;

        case 8:
        errorInfo = "Error Type at line" + strLineNo + ":" +errorType[7];
        break;

        case 9:
        errorInfo = "Error Type at line" + strLineNo + ":" \
        +ObjectName + errorType[8];
        break;

        case 10:
        errorInfo = "Error Type at line" + strLineNo + ":" \
        +"\"" + ObjectName + errorType[9];
        break;

        case 11:
        errorInfo = "Error Type at line" + strLineNo + ":" \
        +"\"" + ObjectName + errorType[10];
        break;

        case 12:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            + "\"" + ObjectName + errorType[11];
        break;

        case 13:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            +errorType[12];

        case 14:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            + errorType[13] + ObjectName + "\"";

        case 15:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            +errorType[14] + ObjectName + "\"";

        case 16:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            +errorType[15] + ObjectName + "\"";

        case 17:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            + errorType[16] + ObjectName + "\"";

        case 18:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            + errorType[17] + ObjectName + "\"";

        case 19:
        errorInfo = "Error Type at line" + strLineNo + ":" \
            + errorType[18] + ObjectName + "\"";

    }
}
