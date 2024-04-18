%{
    #include<stdio.h>
    #include"AstNode.hpp"
    #include "parser.hpp"
    #include "ErrorCode.h"
    #include "Globals.hpp"

    extern "C" int yywrap() { return 1; }

    extern char *filename;

    int yyerror(char const *msg)
    {
        if(filename) {
            fprintf(stderr, "%s:%d %s : %s\n", filename, yylineno, RED("Error") , msg);
        } else {
            fprintf(stderr, "line %d %s : %s\n", yylineno, RED("Error"), msg);
        }
        exit(ERROR_CODE::SYNTAX_ERROR);
    }
%}

%%
"fn" { return T_FN; }
"->" { return T_ARROW; }
"(" { return T_LPAREN; }
")" { return T_RPAREN; }
"{" { return T_LBRACE; }
"}" { return T_RBRACE; }
[0-9]+ {yylval.ival = atoi(yytext); return T_INT;}
[[:alpha:]_][[:alnum:]_]* {yylval.str = strdup(yytext); return T_ID;}

[ \t\n] { /* ignore whitespace */ }
. { exit(ERROR_CODE::LEX_ERROR); }
%%
