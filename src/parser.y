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
%token T_FN T_VAR T_RET T_IF T_ELSE
%token T_LPAREN T_RPAREN T_ARROW T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET T_ASSIGN T_SEMICOLON T_COMMA
%token T_EQ T_NEQ
%token T_PLUS T_MINUS T_MUL T_DIV T_MOD T_LSHIFT T_RSHIFT


/* define the type of the non-terminal */
%type <stmt> statement function_declaration
%type <stmts> statements program

%type <expr> expression call

%type <instr> instr
%type <instrs> instrs

%type <identifier> identifier
%type <integer> integer

/* Binop Precedence */
%left T_EQ T_NEQ
%left T_LSHIFT T_RSHIFT
%left T_MOD
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

args: identifier
    | identifier T_COMMA args
    ;

function_declaration: T_FN identifier T_LPAREN T_RPAREN T_ARROW  T_LBRACE instrs T_RBRACE { $$ = new FunctionDeclaration($2, $7);}
    | T_FN identifier T_LPAREN args T_RPAREN T_ARROW  T_LBRACE instrs T_RBRACE { $$ = new FunctionDeclaration($2, $8) ;}
    ;

instrs: instr {$$ = new InstrList(); $$->push_back($1);}
    | instrs instr {$$ = $1; $1->push_back($2);}
    ;

instr:T_RET expression T_SEMICOLON {$$ = new Return_Instr($2);}
    | T_VAR identifier T_ASSIGN expression T_SEMICOLON {$$ = new Decl_Instr($2, $4);}
    | identifier T_ASSIGN expression T_SEMICOLON {$$ = new Assign_Instr($1, $3);}
    | identifier T_LPAREN T_RPAREN T_SEMICOLON {$$ = new Call_Instr($1);}
    | T_IF T_LBRACKET expression T_RBRACKET T_LBRACE instrs T_RBRACE T_ELSE T_LBRACE instrs T_RBRACE {$$ = new If_Instr($3, $6, $10);}
    ;

expression: integer {$$ = $1;}
    | identifier {$$ = $1;}
    | call {$$ = $1;}
    | expression T_PLUS expression {$$ = new Binop_Expr($1, $3, Binop::Plus);}
    | expression T_MINUS expression {$$ = new Binop_Expr($1, $3, Binop::Minus);}
    | expression T_MUL expression {$$ = new Binop_Expr($1, $3, Binop::Times);}
    | expression T_DIV expression {$$ = new Binop_Expr($1, $3, Binop::Divide);}
    | expression T_MOD expression {$$ = new Binop_Expr($1, $3, Binop::Modulo);}
    | expression T_LSHIFT expression {$$ = new Binop_Expr($1, $3, Binop::Lsl);}
    | expression T_RSHIFT expression {$$ = new Binop_Expr($1, $3, Binop::Lsr);}
    | expression T_EQ expression {$$ = new Binop_Expr($1, $3, Binop::Equal);}
    | expression T_NEQ expression {$$ = new Binop_Expr($1, $3, Binop::NotEqual);}
    | T_LPAREN expression T_RPAREN {$$ = $2;}
    ;

call_args : expression
    | expression T_COMMA call_args
    ;

call: identifier T_LPAREN T_RPAREN {$$ = new Call_Expr($1);}
    | identifier T_LPAREN call_args T_RPAREN {$$ = new Call_Expr($1);}
    ;

integer: T_INT {$$ = new Integer($1);}
    ;

identifier: T_ID {$$ = new Identifier($1);}
    ;
%%
