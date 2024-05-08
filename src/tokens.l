%{
    #include<stdio.h>
    #include"AstNode.hpp"
    #include "parser.hpp"
    #include "ErrorCode.h"
    #include "Globals.hpp"

    extern "C" int yywrap() { return 1; }

    int lex_error(const char *msg, char * text) {
        if(filename) {
            fprintf(stderr, "%s:%d %s : %s %s\n", filename, yylineno, RED("Error") , msg, text);
        } else {
            fprintf(stderr, "line %d %s : %s %s\n", yylineno, RED("Error"), msg, text);
        }
        exit(ERROR_CODE::LEX_ERROR);
    }

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
%option yylineno

%%
"var" { return T_VAR; }
"fn" { return T_FN; }
"ret" { return T_RET; }
"->" { return T_ARROW; }
"(" { return T_LPAREN; }
")" { return T_RPAREN; }
"{" { return T_LBRACE; }
"}" { return T_RBRACE; }
"=" { return T_EQUAL; }
";" { return T_SEMICOLON; }
[0-9]+ {yylval.ival = atoi(yytext); return T_INT;}
[[:alpha:]_][[:alnum:]_]* {yylval.str = strdup(yytext); return T_ID;}

[ \t\n] { /* ignore whitespace */ }
. { lex_error("unexpected character", strdup(yytext)); }
%%
