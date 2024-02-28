%{
    #include<stdio.h>
    #include "parser.hpp"

    #define RED(x) "\033[0;31m" x "\033[0m"

    extern "C" int yywrap() { return 1; }

    extern char *filename;

    int yyerror(char const *msg)
    {
        if(filename) {
            fprintf(stderr, "%s:%d %s : %s\n", filename, yylineno, RED("Error") , msg);
        } else {
            fprintf(stderr, "line %d %s : %s\n", yylineno, RED("Error"), msg);
        }
        exit(1);
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

%%