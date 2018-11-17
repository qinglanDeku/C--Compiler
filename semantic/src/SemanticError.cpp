#include "SemanticError.h"

/************************************
 class SemanticErrorInfo*/

string SemanticErrorInfo::errorType[19] = {
    "Undefined variable.",      //error type1
    "Undefined function.",      //error type2
    "Redefined variable.",      //error type3
    "Redefined function.",      //error type4
    "Type mismatched for assignment.",      //error type5
    "The left-hand side of an assignment must be a variable.",  //error type6
    "Type mismatched for operands",     //error type7
    ""
}