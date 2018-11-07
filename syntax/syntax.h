#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define Syntactic 0
#define Lexical 1
#define Empty 2
typedef enum LexicalType{
    LID, LINT, LFLOAT, LTYPE, LSEMI, LCOMMA, LASSIGNOP, LRELOP, LPLUS, LMINUS,
    LSTAR, LDIV, LAND, LOR, LDOT, LNOT, LLP, LRP, LLB, LRB, LLC, LRC, LSTRUCT, LRETURN,
    LIF, LELSE, LWHILE                  //ADD "L" TO distinguish from enum type in "syntax.tab.h"
}LexicalType;



struct LexicalUnit{
    LexicalType Lextype;
    union{
        char IDname[33];
        int ival;
        float fval;
    };
};
typedef struct LexicalUnit LexicalUnit;

struct SyntacticUnit{
    char name[33];
};
typedef struct SyntacticUnit SyntacticUnit;


typedef union Unit{
    LexicalUnit LU;
    SyntacticUnit SU;
}Unit;




struct SyntaxTreeNode{
    char type;      //0 means Syntactic unit, 1 means Lexical unit, 2 means Empty
    Unit NodeUnit;  //lexical or syntactic
    struct SyntaxTreeNode* child;
    struct SyntaxTreeNode* NextSibling;
    int depth;      //Node's depth
    int lineno;       //line number in code
};
typedef struct SyntaxTreeNode SyntaxTreeNode;

             
//point to Root;


int strtoi(char* str, int Decimal);

void SetRoot(SyntaxTreeNode * node);
//set a node as the root node;

void SetNodeDepth(SyntaxTreeNode* node, int height);
//set a Node's height while taversing

SyntaxTreeNode* CreateNode(char type, Unit NodeUnit, int lineno);      
//Create a Node

void AddChild(SyntaxTreeNode* parent, SyntaxTreeNode* child);  
//set child as parent's child


void AddSibling(SyntaxTreeNode* Node1, SyntaxTreeNode* Node2);
// Set Node2 as Node1's NextSibling

void TraverseTree();
// use with TraversesubTree

void TraverseSubTree(SyntaxTreeNode *node);
//TraversesubTree and set depth

void PrintTree();  
// Print whole Tree

void PrintNode(const SyntaxTreeNode* node);
//Print a Node ,it's children, and it's sibling;

#endif