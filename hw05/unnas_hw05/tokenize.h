#ifndef TOKEN_PARSING_H
#define TOKEN_PARSING_H

#include "svec.h"

void tokenize_and_rev_print(const char* line);
svec* tokenize(const char* line);


char* parse_number(const char* text, long ii);
char* parse_arg_token(const char* text, long ii);
char* parse_operator(const char* text, long ii);

int isoperator(char c);
int isbooloperator(char c);
int isnullterminator(char c);

#endif