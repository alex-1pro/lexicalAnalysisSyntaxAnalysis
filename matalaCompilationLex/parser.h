#pragma once
#define _CRT_SECURE_NO_WARNINGS

void match(int token);
void recovery(int* followArr, int sizeArr);
void tokenError(int firstArr[], int sizeArr, int currLineNumber, int currTokenKind, char* currTokenLexeme);

void parser();
void parse_PROG();
void parse_GLOBAL_VARS();
void parse_GLOBAL_VARS_T();
void parse_VAR_DEC();
void parse_VAR_DEC_T();
void parse_TYPE();
void parse_DIM_SIZES();
void parse_DIM_SIZES_T();
void parse_FUNC_PROTOTYPE();
void parse_FUNC_FULL_DEFS();
void parse_FUNC_FULL_DEFS_T();
void parse_FUNC_WITH_BODY();
void parse_RETURNED_TYPE();
void parse_PARAMS();
void parse_PARAM_LIST();
void parse_PARAM_LIST_T();
void parse_PARAM();
void parse_PARAM_T();
void parse_COMP_STMT();
void parse_VAR_DEC_LIST();
void parse_VAR_DEC_LIST_T();
void parse_STMT_LIST();
void parse_STMT_LIST_T();
void parse_STMT();
void parse_STMT_T();
void parse_IF_STMT();
void parse_ARGS();
void parse_ARG_LIST();
void parse_ARG_LIST_T();
void parse_RETURN_STMT();
void parse_RETURN_STMT_T();
void parse_VAR_T();
void parse_EXPR_LIST();
void parse_EXPR_LIST_T();
void parse_CONDITION();
void parse_EXPR();
void parse_EXPR_T();
void parse_TERM();
void parse_TERM_T();
void parse_FACTOR();
void parse_FACTOR_T();
