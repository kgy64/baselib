%{
#include "ConfigDriver.h"
static int yylex(yy::ConfParser::semantic_type * yylval, yy::ConfParser::location_type * yylloc, ConfDriver & driver);
%}

%require "2.3"
%skeleton "lalr1.cc"
%defines

%define "parser_class_name" "ConfParser"
%parse-param { class ConfDriver& driver }
%lex-param { class ConfDriver& driver }
%error-verbose
%locations

%union {
    ConfAssign * assign;
    AssignmentSet * body;
    ConfigValue * name;
    ConfigLevel * level;
}

%token <name> NAME
%token _AGAIN

%type <name> EXP;
%type <body> BODY;
%type <assign> ASSIGN;
%type <level> DECLARATION;

%destructor { delete($$); } NAME;
%destructor { delete($$); } EXP;
%destructor { delete($$); } BODY;
%destructor { delete($$); } ASSIGN;
%destructor { delete($$); } DECLARATION;

%start eleje

%%

eleje:      // Zero content is also allowed
    | BODY                          { driver.Declare($1); }
    ;

BODY:                                        {  $$ = new AssignmentSet(); }
    | ASSIGN                                 { ($$ = new AssignmentSet())->Append($1); }
    | DECLARATION                            { ($$ = new AssignmentSet())->Append($1); }
    | BODY ASSIGN                            {  $$ = $1->Append($2); }
    | BODY DECLARATION                       {  $$ = $1->Append($2); }
    | BODY error { driver.AddError(); } BODY {  $$ = $1->Append($4); }
    ;

DECLARATION:
      NAME '{' BODY '}'             { $$ = new ConfigLevel(*$1, $3); }
    ;

ASSIGN:
      NAME '=' EXP close            { $$ = new ConfAssign(*$1, *$3); }
    ;

close: ';'  /* mandatory */
    ;

EXP:
      NAME
    ;

%%

static int yylex(yy::ConfParser::semantic_type * yylval, yy::ConfParser::location_type * yylloc, ConfDriver & driver)
{
 return driver.yylex(*yylval, *yylloc);
}

void yy::ConfParser::error (const yy::ConfParser::location_type& l, const std::string& m)
{
 driver.error(l, m);
}
