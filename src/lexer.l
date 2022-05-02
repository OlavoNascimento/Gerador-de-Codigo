%{
    #include <string.h>
    #include <parser.tab.h>
%}

%option noyywrap
%option nounput noinput
%option outfile="lexer.y.c"

IDENTIFIER [_a-zA-Z_][a-zA-Z0-9_]*

%%

"AST -->" {
    return AST_DEFINITION;
}

"TYPE:"	{
    return TYPE;
}

"int" {
    return INT;
}

"void" {
    return VOID;
}

"char" {
    return CHAR;
}

"FUNCTION:" {
    return FUNCTION;
}

"PARAMETER:" {
    return PARAMETER;
}

"RETURN_TYPE:" {
    return RETURN_TYPE;
}

"END_FUNCTION" {
    return END_FUNCTION;
}

"VARIABLE:" {
    return VARIABLE;
}

"IF" {
    return IF;
}

"PRINTF" {
    return PRINTF;
}

"SCANF"	{
    return SCANF;
}

"("	{
    return L_PAREN;
}

")"	{
    return R_PAREN;
}

";"	{
    return SEMI_COLON;
}

"," {
    return COMMA;
}

"&" {
    return AND;
}

"&&" {
    return LOGICAL_AND;
}

"||" {
    return LOGICAL_OR;
}

"<" {
    return LESS_THAN;
}

">" {
    return GREATER_THAN;
}

"<=" {
    return LESS_EQUAL;
}

">=" {
    return GREATER_EQUAL;
}

"==" {
    return EQUAL;
}

"*" {
    return MULTIPLY;
}

"+" {
    return PLUS;
}

"-" {
    return MINUS;
}

"RETURN" {
    return RETURN;
}

{IDENTIFIER} {
    yylval.text = yytext;
    return IDENTIFIER;
}

\'.*\' {
    return CHAR;
}

[0-9]+ {
    yylval.number = atoi(yytext);
    return NUMBER;
}

["]([^\"]|\.)*["] {
    yylval.text = yytext;
    return STRING;
}

(" "|[\t]|[\n]) {}

. {
    printf("ERROR: lexer found an invalid character: %s\n", yytext);
    exit(1);
}

%%
