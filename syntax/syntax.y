/*define && include*/
%{
    extern "C" int yylex(void);
    #include "syntax.h"
    #define YYSTYPE SyntaxTreeNode*
    //#define YYDEBUG 1
    /*include for flex*/
    #include"lex.yy.c"
    extern int syntaxError;
    



    /*define for attribution*/
   

    /*these defines for RELOP*/
    #define G 0
    #define L 1
    #define GE 2
    #define LE 3
    #define E 4
    #define NE 5

    /*these defines for TYPE*/
    #define I 1
    #define F 2

    int yyerror(const char* msg);


%}




/*declare types*/


/*declare token*/
%token INT
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token DOT
%token NOT
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
%token TYPE



/*decalre for non-terminals*/




/*calculate priority*/


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left DIV STAR
%right NOT 
%left DOT LB RB LP RP

%%
/*High-level definition*/
Program: ExtDefList {
        Unit temp;
        strcpy(temp.SU.name, "Program");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        SetRoot($$);
    }
    ;
ExtDefList: /*empty*/   {
        Unit temp;
        strcpy(temp.SU.name, "ExtDefList");
        $$ = CreateNode(Empty, temp, @$.first_line);
    }
    | ExtDef ExtDefList {
        Unit temp;
        strcpy(temp.SU.name, "ExtDefList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }
    ;
ExtDef:Specifier error SEMI |
        Specifier error FunDec CompSt
    |Specifier ExtDecList SEMI   {
        Unit temp;
        strcpy(temp.SU.name, "ExtDef");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Specifier SEMI    {
        Unit temp;
        strcpy(temp.SU.name, "ExtDef");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }

    | Specifier FunDec SEMI     {
        Unit temp;
        strcpy(temp.SU.name, "ExtDef");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Specifier FunDec CompSt   {
        Unit temp;
        strcpy(temp.SU.name, "ExtDef");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;
ExtDecList: VarDec  {
        Unit temp;
        strcpy(temp.SU.name, "ExtDecList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }   
    | VarDec COMMA ExtDecList   {
        Unit temp;
        strcpy(temp.SU.name, "ExtDecList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;

/*Specifier*/
Specifier: 
    TYPE {
        Unit temp;
        strcpy(temp.SU.name, "Specifier");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
}
    | StructSpecifier   {
        Unit temp;
        strcpy(temp.SU.name, "Specifier");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    {
        Unit temp;
        strcpy(temp.SU.name, "StructSpecifier");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
        AddSibling($4, $5);
    }
    | STRUCT Tag    {
        Unit temp;
        strcpy(temp.SU.name, "StructSpecifier");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }
    ;
OptTag: /*empty*/   {
        Unit temp;
        strcpy(temp.SU.name, "OptTag");
        $$ = CreateNode(Empty, temp, @$.first_line);
    }
    | ID    {
        Unit temp;
        strcpy(temp.SU.name, "OptTag");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    ;
Tag: ID {
        Unit temp;
        strcpy(temp.SU.name, "Tag");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    ;


/*Declarator*/
VarDec:ID  {
        Unit temp;
        strcpy(temp.SU.name, "VarDec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }   
    | VarDec LB INT RB  {
        Unit temp;
        strcpy(temp.SU.name, "VarDec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
    }
    ;
FunDec: ID LP VarList RP    {
        Unit temp;
        strcpy(temp.SU.name, "FunDec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
    }
    | ID LP RP  {
        Unit temp;
        strcpy(temp.SU.name, "FunDec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;
VarList: ParamDec COMMA VarList {
        Unit temp;
        strcpy(temp.SU.name, "VarList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
}
    | ParamDec  {
        Unit temp;
        strcpy(temp.SU.name, "VarList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    ;
ParamDec: Specifier VarDec  {
        Unit temp;
        strcpy(temp.SU.name, "ParamDec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
}

    ;


/*Statements*/
CompSt: error RC {
        //printf("Compt RC\n");
    }
    | LC DefList StmtList RC  {
    Unit temp;
        strcpy(temp.SU.name, "CompSt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
}
    ;
StmtList: Stmt StmtList {
    Unit temp;
        strcpy(temp.SU.name, "StmtList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
}
    | /*empty*/ {
        Unit temp;
        strcpy(temp.SU.name, "StmtList");
        $$ = CreateNode(Empty, temp, @$.first_line);
    }
    ;
Stmt: Exp SEMI  {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
}
    | CompSt    {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    | RETURN Exp SEMI   {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | IF LP Exp RP Stmt     %prec LOWER_THAN_ELSE   {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
        AddSibling($4, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
        AddSibling($4, $5);
        AddSibling($5, $6);
        AddSibling($6, $7);
    }
    | WHILE LP Exp RP Stmt  {
        Unit temp;
        strcpy(temp.SU.name, "Stmt");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
        AddSibling($4, $5);
    }
    |IF LP error RP Stmt %prec LOWER_THAN_ELSE |
     IF LP error RP Stmt ELSE Stmt| error SEMI|
    RETURN error SEMI| WHILE LP error RP Stmt
    ;


/*Local Definitions*/
DefList: Def DefList    {
        Unit temp;
        strcpy(temp.SU.name, "DefList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
}
    | /*empty*/ {
        Unit temp;
        strcpy(temp.SU.name, "DefList");
        $$ = CreateNode(Empty, temp, @$.first_line);
    }
    ;
Def: Specifier error SEMI { 
    //printf("Def SEMI\n");  
    }
    |Specifier DecList SEMI {//printf("line : %d\n", @1.first_line);
        Unit temp;
        strcpy(temp.SU.name, "Def");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;
DecList:  Dec  {
        Unit temp;
        strcpy(temp.SU.name, "DecList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
}
    | Dec COMMA DecList {
        Unit temp;
        strcpy(temp.SU.name, "DecList");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;
Dec:VarDec {
        Unit temp;
        strcpy(temp.SU.name, "Dec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
}
    | VarDec ASSIGNOP Exp  {
        Unit temp;
        strcpy(temp.SU.name, "Dec");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    ;


/*Expressions*/
/*Symbol behind error would better be terminal Symbol */
Exp:Exp LB error RB       
    
    | LP error  RP
    |error ASSIGNOP Exp
    | error AND Exp
    | error OR Exp
    | error RELOP Exp


    | Exp ASSIGNOP Exp {
    Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
}
    | Exp AND Exp   {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp OR Exp    {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp RELOP Exp {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp PLUS Exp   {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp MINUS Exp {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp STAR Exp  {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp DIV Exp   {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | LP Exp RP {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | MINUS Exp    {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }
    | PLUS Exp  {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }
    | NOT Exp   {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
    }
    | ID LP Args RP {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
    }
    | ID LP RP  {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | Exp LB Exp RB {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
        AddSibling($3, $4);
    }
    | Exp DOT ID    {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);
    }
    | ID    {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    | INT      {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    } 
    | FLOAT    {
        Unit temp;
        strcpy(temp.SU.name, "Exp");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
    }
    ;
Args: Exp COMMA Args    {
        Unit temp;
        strcpy(temp.SU.name, "Args");
        $$ = CreateNode(Syntactic, temp, @$.first_line);
        AddChild($$, $1);
        AddSibling($1, $2);
        AddSibling($2, $3);    
    }
    |Exp   {Unit temp;
            strcpy(temp.SU.name, "Args");
            $$ = CreateNode(Syntactic, temp, @$.first_line);
            AddChild($$, $1);

            }
    ;


%%

int yyerror(const char *msg){
    printf("Error Type B at line %d.\n", yyline);
    syntaxError = 1;
}
/*error insert in :
    Stmt 
    CompSt
    Exp
    Def
    ExtDef
*/
