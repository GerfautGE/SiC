%code requires {
}

%{
    #include <iostream>
    #include "AstNode.hpp"
    #include "FunctionDeclaration.hpp"
    #include "Globals.hpp"

    extern int yylex();
    int yyerror(char const *msg);
    #define YYERROR_VERBOSE
    #define YYDEBUG 1


    Block *programBlock; /* the top level root node of our final AST */
%}

%union {
    Node *node;
    Block *block;
    Expression *expr;
    Statement *statement;
    Identifier *identifier;
    int ival;
    char *str;
    int token;
}

/* define tokens. MUST be the same than in tokens.l */ 
%token <str> T_ID
%token <ival> T_INT
%token T_FN T_LPAREN T_RPAREN T_ARROW T_LBRACE T_RBRACE

/* define the type of the non-terminal */
%type <block> program statement_list block
%type <statement> statement function_declaration
%type <expr> expr integer
%type <identifier> identifier


%start program
%debug
%verbose
%locations

%header

%%
program: statement_list { programBlock = $1; }
    | %empty  { programBlock = new Block(); }
    ;

statement_list: statement {$$ = new Block(); $$->statements.push_back($<statement>1);}
    | statement_list statement { $1->statements.push_back($<statement>2);}
    ;

statement: function_declaration
    ;

function_declaration: T_FN identifier T_LPAREN T_RPAREN T_ARROW T_LBRACE block T_RBRACE { $$ = new FunctionDeclaration($2, $7); }
    ;

block: return
    ;

return: expr
    ;

expr: integer
    ;

integer: T_INT {$$ = new Integer($1);}
    ;

identifier: T_ID {$$ = new Identifier($1);}
    ;
%%