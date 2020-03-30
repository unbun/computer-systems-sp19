#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "svec.h"
#include "tokenize.h"

void
tokenize_and_print(const char* line) {
	// tokenize and print the given line in reverse
	svec* tokens = tokenize(line);

	for(int ii = 0; ii < tokens->size; ii++) {
		char* tt = svec_get(tokens, ii);
		puts(tt);
	}

	free_svec(tokens); // frees the svec and any malloc/buffers it had in data
}

void
tokenize_and_rev_print(const char* line) {
	// tokenize and print the given line in reverse
	svec* tokens = tokenize(line);

	for(int ii = tokens->size - 1; ii >= 0; ii--) {
		char* tt = svec_get(tokens, ii);
		puts(tt);
	}

	free_svec(tokens); // frees the svec and any malloc/buffers it had in data
}

svec*
tokenize(const char* line) {

	svec* xs = make_svec();

	long nn = strlen(line);
	long ii = 0;

	/* Structure from Lecture 09*/
	while(ii < nn) {

		//if space, move on
		if(isspace(line[ii])) {
			ii++;
			continue;
		}

		//if operator, read it and any boolean operators following as one token, ii+= op_length
		if(isoperator(line[ii])) {
			char* op = parse_operator(line, ii);
			svec_push_back(xs, op);
			ii += strlen(op);
			continue;
		}

		//if number, read it and any numbers following as one token, ii+= number_length
		if(isdigit(line[ii])) {
			char* num = parse_number(line, ii);
			svec_push_back(xs, num);
			ii += strlen(num);
			continue;
		}

		//else, it's an argument, read it and any valid chars following it as a token, ii+=string_length
		char* arg = parse_arg_token(line, ii);
		svec_push_back(xs, arg);
		ii += strlen(arg);
	}

	return xs;
}

char*
parse_number(const char* text, long ii)
{
	// text: the line to parse
	// ii: the starting index to parse the line at

	/* From Lecture 09*/
	int nn = 0;
    while (isdigit(text[ii + nn])) {
        ++nn;
    }

    char* num = malloc(nn + 1);
    memcpy(num, text + ii, nn);
    num[nn] = 0;
    return num;
}

char*
parse_arg_token(const char* text, long ii)
{
	// text: the line to parse
	// ii: the starting index to parse the line at
	
	int nn = 0;
    while (!isspace(text[ii + nn]) 
    	&& !isoperator(text[ii + nn])
    	&& !isnullterminator(text[ii + nn])) {
        ++nn;
    }

    char* tt = malloc(nn + 1);
    memcpy(tt, text + ii, nn);
    tt[nn] = 0;
    return tt;
}

char*
parse_operator(const char* text, long ii)
{
	// text: the line to parse
	// ii: the starting index to parse the line at

	int nn = 1;
	if(isbooloperator(text[ii + 1])
		&& text[ii] == text[ii + 1]) {
		nn = 2;
	}

	char *tt = malloc(nn);
	memcpy(tt, &text[ii], nn);
	tt[nn] = 0;
    return tt;
}

int
isoperator(char c)
{
	return c == '<' || c == '>' 
		|| c == '|' || c == '&' 
		|| c == ';';
}

int
isbooloperator(char c)
{
	return c == '|' || c == '&';
}

int
isnullterminator(char c)
{
	return c == '\0';
}
