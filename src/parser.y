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
%token T_PLUS T_MINUS T_MUL T_DIV

/* define the type of the non-terminal */
%type <stmt> statement function_declaration
%type <stmts> statements program

%type <expr> expression call

%type <instr> instr
%type <instrs> instrs

%type <identifier> identifier
%type <integer> integer

/* Binop Precedence */
%left T_PLUS T_MINUS
%left T_MUL T_DIV

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

instr:T_RET expression T_SEMICOLON {$$ = new Return_Instr($2);}
    | T_VAR identifier T_EQUAL expression T_SEMICOLON {$$ = new Decl_Instr($2, $4);}
    | identifier T_LPAREN T_RPAREN T_SEMICOLON {$$ = new Call_Instr($1);}
    ;

expression: integer {$$ = $1;}
    | identifier {$$ = $1;}
    | call {$$ = $1;}
    | expression T_PLUS expression {$$ = new Binop_Expr($1, $3, Binop::Plus);}
    | expression T_MINUS expression {$$ = new Binop_Expr($1, $3, Binop::Minus);}
    | expression T_MUL expression {$$ = new Binop_Expr($1, $3, Binop::Times);}
    | expression T_DIV expression {$$ = new Binop_Expr($1, $3, Binop::Divide);}
    | T_LPAREN expression T_RPAREN {$$ = $2;}
    ;

call: identifier T_LPAREN T_RPAREN {$$ = new Call_Expr($1);}
    ;

integer: T_INT {$$ = new Integer($1);}
    ;

identifier: T_ID {$$ = new Identifier($1);}
    ;
%%
