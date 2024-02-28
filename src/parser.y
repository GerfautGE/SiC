%code requires {
}

%{
    #include <iostream>
    #include "AstNode.hpp"

    extern int yylex();
    int yyerror(char const *msg);
    #define YYERROR_VERBOSE
    #define YYDEBUG 1
%}

%union {
    int ival;
    char *str;
}

/* define tokens. MUST be the same than in tokens.l */ 
%token <str> T_ID
%token <ival> T_INT
%token T_FN T_ARROW T_LPAREN T_RPAREN T_LBRACE T_RBRACE

/* define the type of the YYSTYPE */
%type <str> identifier
%type <ival> integer

%start program
%debug
%verbose
%locations

%header

%%
program: statement_list
    | %empty
    ;

statement_list: statement
    | statement_list statement
    ;

statement: function_declaration
    ;

function_declaration: T_FN identifier T_LPAREN T_RPAREN T_ARROW T_LBRACE block T_RBRACE
    ;

block: expr_list
    ;

expr_list: expr
    | expr_list expr
    ;

expr: integer
    ;

integer: T_INT
    ;

identifier: T_ID
    ;
%%