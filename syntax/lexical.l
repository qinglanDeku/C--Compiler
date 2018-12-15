%{
        extern "C" int yylex(void);
        #include"syntax.h"
        #include"syntax.tab.h"
        #define YYSTYPE SyntaxTreeNode*
        extern int yycolumn;
        extern int yyline;
        extern int lexicalError;
        #define YY_USER_ACTION\
          yylloc.first_line = yylloc.last_line = yyline;\
          yylloc.first_column = yycolumn;\
          yylloc.last_column = yycolumn + yyleng - 1;\
          yycolumn += yyleng;
%}


skip [ \n]
NOTE1 "//".*
NOTE2 "/*"[^*]*"*"+([^*/][^*]*"*"+)*"/"

digit [0-9]
octal_digit[0-7]
Hex_digit [0-9a-fA-F]
_letter [_A-Za-z]
letter [A-Za-z]
digits {digit}+

IDstart [_A-Za-z]
IDtail [_A-Za-z0-9]
Identifier {IDstart}+{IDtail}*

Decimal ([1-9]{digit}*)|"0"
Octal 0{octal_digit}+
HexHead 0[Xx]
Hex {HexHead}{Hex_digit}+

Integer {Hex}|{Decimal}|{Octal}


F_base1 {digits}"."
F_base2 "."{digits}
F_base3 {digits}"."{digits}
F_expsymbol [Ee]
F_exp {F_expsymbol}[+-]?{digits}
Float ({F_base1}|{F_base2}|{F_base3}){F_exp}?

SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP "=="|"!="|"<"|">"|"<="|">="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"

TYPE int|float

LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"

STRUCT "struct"
RETURN "return" 

IF "if"
ELSE "else"
WHILE "while"






%%
{skip} {if(strcmp(" ", yytext) == 0);
        else  {yycolumn= 1;
                yyline += 1;}
        }
{TYPE} {
        Unit temp;
        temp.LU.Lextype = LTYPE;
        strcpy(temp.LU.IDname, yytext); 
        yylval = CreateNode(Lexical, temp, yyline);
        return TYPE;
        }



{SEMI} {Unit temp;
        temp.LU.Lextype = LSEMI;
        yylval = CreateNode(Lexical, temp, yyline);
        return SEMI;}

{COMMA} {Unit temp;
        temp.LU.Lextype = LCOMMA;
        yylval = CreateNode(Lexical, temp, yyline);
        return COMMA;}

{ASSIGNOP} {Unit temp;
        temp.LU.Lextype = LASSIGNOP;
        yylval = CreateNode(Lexical, temp, yyline);
        return ASSIGNOP;}

{RELOP} {Unit temp;
        temp.LU.Lextype = LRELOP;
        strcpy(temp.LU.IDname, yytext);
        yylval = CreateNode(Lexical, temp, yyline);
        return RELOP;}

{PLUS} {Unit temp;
        temp.LU.Lextype = LPLUS;
        yylval = CreateNode(Lexical, temp, yyline);
        return PLUS;}
{MINUS} {Unit temp;
        temp.LU.Lextype = LMINUS;
        yylval = CreateNode(Lexical, temp, yyline);
        return MINUS;}
{STAR} {Unit temp;
        temp.LU.Lextype = LSTAR;
        yylval = CreateNode(Lexical, temp, yyline);
        return STAR;}
{DIV} {Unit temp;
        temp.LU.Lextype = LDIV;
        yylval = CreateNode(Lexical, temp, yyline);
        return DIV;}
{AND} {Unit temp;
        temp.LU.Lextype = LAND;
        yylval = CreateNode(Lexical, temp, yyline);
        return AND;}
{OR} {Unit temp;
        temp.LU.Lextype = LOR;
        yylval = CreateNode(Lexical, temp, yyline);
        return OR;}
{DOT} {Unit temp;
        temp.LU.Lextype = LDOT;
        yylval = CreateNode(Lexical, temp, yyline);
        return DOT;}
{NOT} {Unit temp;
        temp.LU.Lextype = LNOT;
        yylval = CreateNode(Lexical, temp, yyline);
        return NOT;}

{LP} {Unit temp;
        temp.LU.Lextype = LLP;
        yylval = CreateNode(Lexical, temp, yyline);
        return LP;}
{RP} {Unit temp;
        temp.LU.Lextype = LRP;
        yylval = CreateNode(Lexical, temp, yyline);
        return RP;}
{LB} {Unit temp;
        temp.LU.Lextype = LLB;
        yylval = CreateNode(Lexical, temp, yyline);
        return LB;}
{RB} {Unit temp;
        temp.LU.Lextype = LRB;
        yylval = CreateNode(Lexical, temp, yyline);
        return RB;}
{LC} {Unit temp;
        temp.LU.Lextype = LLC;
        yylval = CreateNode(Lexical, temp, yyline);
        return LC;}
{RC} {Unit temp;
        temp.LU.Lextype = LRC;
        yylval = CreateNode(Lexical, temp, yyline);
        return RC;}


{STRUCT} {Unit temp;
        temp.LU.Lextype = LSTRUCT;
        yylval = CreateNode(Lexical, temp, yyline);
        return STRUCT;}
{RETURN} {Unit temp;
        temp.LU.Lextype = LRETURN;
        yylval = CreateNode(Lexical, temp, yyline);
        return RETURN;}

{IF} {Unit temp;
        temp.LU.Lextype = LIF;
        yylval = CreateNode(Lexical, temp, yyline);
        return IF;
        }
{ELSE} {Unit temp;
        temp.LU.Lextype = LELSE;
        yylval = CreateNode(Lexical, temp, yyline);
        return ELSE;}
{WHILE} {Unit temp;
        temp.LU.Lextype = LWHILE;
        yylval = CreateNode(Lexical, temp, yyline);
        return WHILE;}



{Integer} {Unit temp;
        temp.LU.Lextype = LINT;
        if(yytext[0] == '0'){
                if(strlen(yytext) == 1){
                        temp.LU.ival = atoi(yytext);
                }
                else{
                        if(yytext[1] == 'x' || yytext[1] =='X')
                                temp.LU.ival = strtoi(yytext, 16);
                        else
                                temp.LU.ival = strtoi(yytext, 8);

                }
        }
        else 
                temp.LU.ival = atoi(yytext);
        
        yylval = CreateNode(Lexical, temp, yyline);
              return INT;  }
{Float} {Unit temp;
        temp.LU.Lextype = LFLOAT;
        temp.LU.fval = atof(yytext);
        yylval = CreateNode(Lexical, temp, yyline);
        return FLOAT;}

{Identifier} {Unit temp;
        temp.LU.Lextype = LID;
        strcpy(temp.LU.IDname, yytext);
        yylval = CreateNode(Lexical, temp, yyline); 
        return ID;}

{NOTE1} {}
{NOTE2} {
         for(int i = 0 ; i < strlen(yytext); i++)
                if(yytext[i] == '\n') yyline += 1;
         }
    

. {printf("Error type A at line %d: Mysterious character \"%s\" .\n", yyline, yytext);
        lexicalError = 1; }
%%