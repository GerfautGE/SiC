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

%}

%union {
    Program *rootAST;

    Statement *stmt;
    StatementList *stmts;

    Instr *instr;
    InstrList *instrs;
    Expression *expr;

    Identifier *identifier;
    Integer *integer;

    int ival;
    char *str;
    int token;
}

/* define tokens. MUST be the same than in tokens.l */
%token <str> T_ID
%token <ival> T_INT
%token T_FN T_LPAREN T_RPAREN T_ARROW T_LBRACE T_RBRACE T_VAR T_EQUAL T_SEMICOLON T_RET

/* define the type of the non-terminal */
%type <stmt> statement function_declaration
%type <stmts> statements program

%type <expr> expression literal call

%type <instr> instr return_instr decl_instr call_instr
%type <instrs> instrs

%type <identifier> identifier
%type <integer> integer

%start program
%debug
%verbose
%locations
%header

%%
program: statements {rootAST = new Program($1);}
    ;

statements: statement {$$ = new StatementList(); $$->push_back($1);}
    | statements statement {$$ = $1; $1->push_back($2);}
    ;

statement: function_declaration {$$ = $1;}
    ;

function_declaration: T_FN identifier T_LPAREN T_RPAREN T_ARROW  T_LBRACE instrs T_RBRACE
    {
        $$ = new FunctionDeclaration($2, $7);
    }
    ;

instrs: instr {$$ = new InstrList(); $$->push_back($1);}
    | instrs instr {$$ = $1; $1->push_back($2);}
    ;

instr: return_instr {$$ = $1;}
    | decl_instr T_SEMICOLON {$$ = $1;}
    | call_instr T_SEMICOLON {$$ = $1;}
    ;

return_instr: expression {$$ = new Return_Instr($1);}
    | T_RET expression T_SEMICOLON {$$ = new Return_Instr($2);}
    ;

decl_instr: T_VAR identifier T_EQUAL expression {$$ = new Decl_Instr($2, $4);}
    ;

call_instr: identifier T_LPAREN T_RPAREN {$$ = new Call_Instr($1);}
    ;

literal: integer {$$ = $1;}
    ;

expression: literal {$$ = $1;}
    | identifier {$$ = $1;}
    | call {$$ = $1;}
    ;

call: identifier T_LPAREN T_RPAREN {$$ = new Call_Expr($1);}

integer: T_INT {$$ = new Integer($1);}
    ;

identifier: T_ID {$$ = new Identifier($1);}
    ;
%%
