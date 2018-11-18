#include"syntax.h"

SyntaxTreeNode *TreeRoot;  
char LexicalStrs[32][32] = {
    "ID", "INT", "FLOAT", "TYPE", "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", "MINUS",
    "STAR", "DIV", "AND", "OR", "DOT", "NOT", "LP", "RP", "LB", "RB", "LC", "RC", "STRUCT", "RETURN",
    "IF", "ELSE", "WHILE"
};

int mypow(int base, int exp){
    int retval = 1;
    for(int i = 0; i < exp; i++)
        retval *= base;
        
    //printf("mypow %d\n", retval);
    return retval;
}

int strtoi(char *str, int Decimal){
    int retval = 0;
    //printf("%s,%d\n", str,strlen(str));
    int length = strlen(str);
    if(Decimal == 8){
        for(int i = 1 ; i<length  ; i++){
            retval += (int)(str[i] - '0')*mypow(8, length -i -1 );
            // printf("%d\n",retval);
            }
    }
    else{
        for(int i = 2; i< length; i++){
            if(str[i]<='f' && str[i] >='a')
                retval += (int)(str[i] - 'a' + 10)* mypow(16, (length -i -1));
            
            else if(str[i] <='F' && str[i] >='A')
                retval += (int)(str[i] - 'A' + 10)* mypow(16, (length -i -1));
            
            else
                retval += (int)(str[i] - '0')* mypow(16, (length -i -1));
        }
    }
    //printf("%d, %d\n",Decimal,retval);
    return retval;
}

void PrintLex(const SyntaxTreeNode* LexNode){

    printf("%s", LexicalStrs[LexNode->NodeUnit.LU.Lextype]);
    if(LexNode->NodeUnit.LU.Lextype == LID)
        printf(": %s", LexNode->NodeUnit.LU.IDname);
    else if(LexNode->NodeUnit.LU.Lextype == LINT)
        printf(": %d", LexNode->NodeUnit.LU.ival);
    else if(LexNode->NodeUnit.LU.Lextype == LFLOAT)
        printf(": %f", LexNode->NodeUnit.LU.fval);
    else if(LexNode->NodeUnit.LU.Lextype == LTYPE)
        printf(": %s", LexNode->NodeUnit.LU.IDname);
    else    ;
    printf("\n");

}

void PrintSyntax(const SyntaxTreeNode* SynNode){
    printf("%s", SynNode->NodeUnit.SU.name);
    printf(" (%d)\n", SynNode->lineno);
}



SyntaxTreeNode* CreateNode(char type, Unit NodeUnit, int lineno){
    SyntaxTreeNode* p = (SyntaxTreeNode*)malloc(sizeof(SyntaxTreeNode));
    p->type = type;
    p->NodeUnit = NodeUnit;
    p->child = NULL;
    p->NextSibling = NULL;
    p->depth = 0;
    p->lineno = lineno;
    return p;
}

void AddChild(SyntaxTreeNode* parent, SyntaxTreeNode* child){
    parent->child = child;
}

int ChildNumber(SyntaxTreeNode* parent){
    int mycount = 0;
    SyntaxTreeNode* Cp = parent->child;
    while(Cp != NULL){
        mycount += 1;
        Cp = Cp->NextSibling;
    }
    return mycount;
}

SyntaxTreeNode* GetChild(SyntaxTreeNode* parent, int No){
    SyntaxTreeNode *pChild = parent->child;
    for(int i = 1 ; i < No; i++){
        if(pChild == NULL)
            break;
        pChild = pChild->NextSibling;
    }
    return pChild;
}

void AddSibling(SyntaxTreeNode* Node1, SyntaxTreeNode* Node2){
    Node1->NextSibling = Node2;
}

void SetRoot(SyntaxTreeNode * node){
    TreeRoot = node;
}

void SetNodeDepth(SyntaxTreeNode* node, int depth){
    node->depth = depth;
}



void TraverseTree(){
    TraverseSubTree(TreeRoot);
}

void TraverseSubTree(SyntaxTreeNode *node){
    static int depth = 0;
    if(node == NULL)
        return;
    
    SetNodeDepth(node, depth);
    depth++;


    /*if(node->type == 0){
        node->NodeUnit.SU.name 
    }*/
    TraverseSubTree(node->child);
    depth--;
    TraverseSubTree(node->NextSibling);
}

void PrintTree(){
    PrintNode(TreeRoot);
}

void PrintNode(const SyntaxTreeNode* node){
    //static int depth = 0;
    if(node == NULL)
        return;
    
    //SetNodeDepth(node, depth);
    switch(node->type){
        case Empty: break;
        case Lexical:{
            for(int i = 0; i < node->depth; i++)
                printf(" ");
            PrintLex(node);
            break;
        }
        case Syntactic:{
            for(int i = 0; i < node->depth; i++)
                printf(" ");
            PrintSyntax(node);
        }
    }
   // depth++;
    PrintNode(node->child);
   // depth--;
    PrintNode(node->NextSibling);
}