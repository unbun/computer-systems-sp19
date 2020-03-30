#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "svec.h"

void assert_ok(long rv, char* call);

void execute_cd(svec* cmd);
void execute_base_case(svec* xs);
void execute_redirect_out(svec* tokens);
void execute_redirect_in(svec* tokens);
void execute_pipe(svec* tokens);
void execute_background(svec* tokens);
void execute_logical_and(svec* tokens);
void execute_logical_or(svec* tokens);

#endif