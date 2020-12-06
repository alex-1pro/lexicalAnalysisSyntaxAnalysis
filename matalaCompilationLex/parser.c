#define _CTR_SECURE_NO_WARNINGS

#include "Token.h"
#include "parser.h"

Token *current_t = NULL;


void parser() {
	parse_PROG();
	match(TOKEN_EOF);
}

void tokenError(int firstArr[], int sizeArr, int currLineNumber, int currTokenKind, char* currTokenLexeme)
{
	int i;

	fprintf(xnxxout, "Expected: one of tokens ");
	for (i = 0; i < sizeArr; i++) {
		fprintf(xnxxout, "%s,", tokenStringCases(firstArr[i]));
	}
	fprintf(xnxxout, "Actual token: %s, lexeme: %s, at line %d \n", tokenStringCases(currTokenKind), currTokenLexeme, currLineNumber);
}

void match(int token)
{
	current_t = next_token();
	if (current_t->kind != token)
	{
		fprintf(xnxxout, "Expected token of type {%s} at line : {%d},Actual token of type {%s}, token.lexeme :{%s}\n"
			, tokenStringCases(token), current_t->lineNumber, tokenStringCases(current_t->kind), current_t->lexeme);
	}
}

//PROG->GLOBAL_VARS  FUNC_PREDEFS  FUNC_FULL_DEFS

void parse_PROG()
{
	fprintf(xnxxout, "Rule ( PROG->GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS )\n");
	parse_GLOBAL_VARS();

	do {
		parse_FUNC_PROTOTYPE();
		current_t = next_token();
		/* legally, the next token after FUNC_PROTOTYPE
		   can be either semicolon or {
		   any other token is erroneous/unexpected */
	} while (current_t->kind == TOKEN_SEPARACTION_SIGNS_SEMICOLON);  /* if FUNC_PROTOTYPE is followed by a semicolon,
											then at least one more FUNC_PROTOTYPE is expected */

	if (current_t->kind == TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES)  /* if FUNC_PROTOTYPE is followed by {
												 then actually the first FUNC_WITH_BODY is detected
												 and hence now the body of the function is expected */
	{
		current_t = back_token();        /* because every function parse_X starts itself with a call to next_token  */
		parse_COMP_STMT();       /* body of the first FUNC_WITH_BODY */
		current_t = next_token();
		if (current_t->kind != TOKEN_EOF)    /* something more after the first FUNC_WITH_BODY -
									this can be only FUNC_FULL_DEFS */
		{
			current_t = back_token();     /* because every function parse_X starts itself with a call to next_token  */
			parse_FUNC_FULL_DEFS();
		}
		else
			current_t = back_token();     /* because in the main of parser, match(EOF) appears after the call to parse_PROG(),
									 and function match() itself starts with next_token  */
	}
	else
	{	//error
		fprintf(xnxxout, "FUNC_PROTOTYPE can be followed only by semicolon or by {  ");   /* FUNC_PROTOTYPE can be followed only by semicolon or by {   */
		while (current_t->kind != TOKEN_EOF)
		{
			current_t = next_token();
		}   /* skip till the end of input */
		current_t = back_token();
	}
}  /* end of parse_PROG  */






//GLOBAL_VARS->VAR_DEC GLOBAL_VARS_T

void parse_GLOBAL_VARS() {
	
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	int follow[] = { TOKEN_INT,TOKEN_FLOAT ,TOKEN_VOID };
	current_t = next_token();
	

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule (GLOBAL_VARS->VAR_DEC GLOBAL_VARS_T)\n");
		current_t = back_token();
		parse_VAR_DEC();
		parse_GLOBAL_VARS_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		
		parse_GLOBAL_VARS();
		break;
		
	}

}
//GLOBAL_VARS_T->VAR_DEC GLOBAL_VARS_T|epsilon

void parse_GLOBAL_VARS_T() {
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	int follow[] = { TOKEN_INT,TOKEN_FLOAT ,TOKEN_VOID };
	current_t = next_token();
	switch (current_t->kind)
	{

		//FIRST
	case TOKEN_INT:
	case TOKEN_FLOAT:
		current_t = next_token();
		if (current_t->kind == TOKEN_ID) {
			current_t = next_token();
			if (current_t->kind != TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES) {
				current_t = back_token();
				current_t = back_token();
				current_t = back_token();
				
				fprintf(xnxxout, "Rule (GLOBAL_VARS_T->VAR_DEC GLOBAL_VARS_T)\n");
				parse_VAR_DEC();
				parse_GLOBAL_VARS_T();
				break;
			}
			else {
				current_t = back_token();
				current_t = back_token();
				current_t = back_token();
				break;
			}
		}
		
		//FOLLOW	
	case TOKEN_VOID:

		fprintf(xnxxout, "Rule (GLOBAL_VARS_T->epsilon)\n");
		current_t = back_token();
		break;

	default:

		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}


}


//VAR_DEC -> TYPE id VAR_DEC_T 
void parse_VAR_DEC()
{
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	//float id if int return void {
	int follow[] = { TOKEN_INT,TOKEN_FLOAT ,TOKEN_VOID,TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	current_t = next_token();
	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule (VAR_DEC -> TYPE id VAR_DEC_T)\n ");
		current_t = back_token();
		parse_TYPE();
		match(TOKEN_ID);
		
		parse_VAR_DEC_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//	VAR_DEC_T -> ; | [ DIM_SIZES ] ; 
void parse_VAR_DEC_T()
{
	//{;  [}
	int first[] = { TOKEN_SEPARACTION_SIGNS_SEMICOLON, TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS };
	//float id if int return void{
	int follow[] = { TOKEN_INT,TOKEN_FLOAT ,TOKEN_VOID,TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	current_t = next_token();
	switch (current_t->kind)
	{

	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
		fprintf(xnxxout, "Rule (VAR_DEC_T -> ; )\n");
		
		break;

	case TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS:
		fprintf(xnxxout, "Rule (VAR_DEC_T ->  [ DIM_SIZES ]) \n");
		parse_DIM_SIZES();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS);
		match(TOKEN_SEPARACTION_SIGNS_SEMICOLON);
		break;

	default:
		if (current_t->kind == TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES) {
			current_t = back_token();
			break;
		}
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//TYPE -> int   |  float   
void parse_TYPE()
{
	//{Int ,float}
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	//{id}
	int follow[] = { TOKEN_ID };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule (TYPE -> int | float) \n");
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//DIM_SIZES->int_num  DIM_SIZES_T
void parse_DIM_SIZES()
{
	//{int_num}
	int first[] = { INT_NUM };
	//]
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS };
	current_t = next_token();
	switch (current_t->kind)
	{
	case INT_NUM:
		fprintf(xnxxout, "Rule (DIM_SIZES->int_num  DIM_SIZES_T)\n");
		parse_DIM_SIZES_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//DIM_SIZES_T -> epsilon | , DIM_SIZES	
void parse_DIM_SIZES_T()
{

	int first[] = { TOKEN_SEPARACTION_SIGNS_COMMA };
	
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS };
	current_t = next_token();
	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_SEPARACTION_SIGNS_COMMA:
		fprintf(xnxxout, "Rule (DIM_SIZES_T -> , DIM_SIZES)\n ");
		parse_DIM_SIZES();
		break;
		//FOLLOW
	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
		fprintf(xnxxout, "Rule (DIM_SIZES_T -> epsilon)\n");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}





//FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS)
void parse_FUNC_PROTOTYPE()
{
	//{int, float, void}
	int first[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID };
	//{;{}
	int follow[] = { TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
	case TOKEN_VOID:                 
		fprintf(xnxxout, "Rule (FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS))\n");

		
		current_t = back_token();
		parse_RETURNED_TYPE();
		match(TOKEN_ID);
		match(TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES);
		parse_PARAMS();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES);
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//	FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_T

void parse_FUNC_FULL_DEFS()
{
	//{int, float, void}
	int first[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID };
	////{EOF}
	int follow[] = { TOKEN_EOF };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
	case TOKEN_VOID:
		fprintf(xnxxout, "Rule (FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_T)\n");
		current_t = back_token();
		parse_FUNC_WITH_BODY();
		parse_FUNC_FULL_DEFS_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}
// FUNC_FULL_DEFS_T -> FUNC_WITH_BODY FUNC_FULL_DEFS_T | epsilon 
void parse_FUNC_FULL_DEFS_T()
{
	//{int, float, void}
	int first[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID };
	////{EOF}
	int follow[] = { TOKEN_EOF };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
	case TOKEN_VOID:
		fprintf(xnxxout, "Rule ( FUNC_FULL_DEFS_T -> FUNC_WITH_BODY FUNC_FULL_DEFS_T  )\n ");
		current_t = back_token();
		parse_FUNC_WITH_BODY();
		parse_FUNC_FULL_DEFS_T();
		break;

	case TOKEN_EOF:
		fprintf(xnxxout, "Rule ( FUNC_FULL_DEFS_T -> epsilon )\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//FUNC_WITH_BODY -> FUNC_PROTOTYPE  COMP_STMT
void parse_FUNC_WITH_BODY()
{
	//{int, float, void}
	int first[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID };
	////{int,float,void,EOF}
	int follow[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID,TOKEN_EOF };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
	case TOKEN_VOID:
		fprintf(xnxxout, "Rule ( FUNC_WITH_BODY -> FUNC_PROTOTYPE  COMP_STMT )\n ");
		current_t = back_token();
		parse_FUNC_PROTOTYPE();
		parse_COMP_STMT();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}

//RETURNED_TYPE->TYPE | void

void parse_RETURNED_TYPE()
{

	//{int, float, void}
	int first[] = { TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID };
	////{id}
	int follow[] = { TOKEN_ID };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( RETURNED_TYPE->TYPE )\n ");
		current_t = back_token();
		parse_TYPE();
		break;

	case TOKEN_VOID:
		fprintf(xnxxout, "Rule ( RETURNED_TYPE-> void )\n ");
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//PARAMS->PARAM_LIST | epsilon
void parse_PARAMS()
{
	//{int, float}
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	////{)}
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( PARAMS->PARAM_LIST )\n ");
		current_t = back_token();
		parse_PARAM_LIST();
		break;
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
		fprintf(xnxxout, "Rule ( PARAMS->epsilon )\n");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}
//PARAM_LIST -> PARAM ARAM_LIST_T
void parse_PARAM_LIST()
{
	//{int, float}
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	////{)}
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( PARAM_LIST->PARAM PARAM_LIST_T )\n");
		current_t = back_token();
		parse_PARAM();
		parse_PARAM_LIST_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//PARAM_LIST_T-> epsilon | , PARAM PARAM_LIST'
void parse_PARAM_LIST_T()
{
	//{,}
	int first[] = { TOKEN_SEPARACTION_SIGNS_COMMA };
	////{)}
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_COMMA:
		fprintf(xnxxout, "Rule ( PARAM_LIST_T-> , PARAM PARAM_LIST' )\n");
		parse_PARAM();
		parse_PARAM_LIST_T();
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
		fprintf(xnxxout, "Rule ( PARAM_LIST_T-> epsilon )\n");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//PARAM -> TYPE id PARAM'
void parse_PARAM()
{
	//{int, float}
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	////{) , }
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES,TOKEN_SEPARACTION_SIGNS_COMMA };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( PARAM -> TYPE id PARAM_T )\n");
		current_t = back_token();
		parse_TYPE();
		match(TOKEN_ID);
		parse_PARAM_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//PARAM_T -> epsilon | [ DIM_SIZES ])
void parse_PARAM_T()
{	//{[}
	int first[] = { TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS };
	////{) , }
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES,TOKEN_SEPARACTION_SIGNS_COMMA };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS:
		fprintf(xnxxout, "Rule ( PARAM_T -> [ DIM_SIZES ] )\n");
		parse_DIM_SIZES();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS);
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
	case TOKEN_SEPARACTION_SIGNS_COMMA:
		fprintf(xnxxout, "Rule ( PARAM_T -> epsilon )\n");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//COMP_STMT -> { VAR_DEC_LIST  STMT_LIST }

void parse_COMP_STMT()
{
	//{ { }
	int first[] = { TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	////{EOF, ; , float ,int ,void ,}, }
	int follow[] = { TOKEN_EOF,TOKEN_INT,TOKEN_FLOAT,TOKEN_VOID ,TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES,TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();
	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES:
		fprintf(xnxxout, "Rule ( COMP_STMT -> { VAR_DEC_LIST  STMT_LIST } )\n");

		parse_VAR_DEC_LIST();
		parse_STMT_LIST();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES);
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//VAR_DEC_LIST-> VAR_DEC_LIST'|epsilon 

void parse_VAR_DEC_LIST()
{
	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	int follow[] = { TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	current_t = next_token();

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( VAR_DEC_LIST -> VAR_DEC  VAR_DEC_LIST_T )\n ");
		current_t = back_token();
		parse_VAR_DEC_LIST_T();
		break;

	case TOKEN_ID:
	case TOKEN_IF:
	case TOKEN_RETURN:
	case TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES:
		fprintf(xnxxout, "Rule ( VAR_DEC_LIST -> epsilon )\n ");
		current_t = back_token();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}

//VAR_DEC_LIST' -> VAR_DEC  VAR_DEC_LIST' | epsilon

void parse_VAR_DEC_LIST_T() {

	int first[] = { TOKEN_INT,TOKEN_FLOAT };
	int follow[] = { TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	current_t = next_token();

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_INT:
	case TOKEN_FLOAT:
		fprintf(xnxxout, "Rule ( VAR_DEC_LIST_T -> VAR_DEC  VAR_DEC_LIST_T)\n ");
		current_t = back_token();
		parse_VAR_DEC();
		
		parse_VAR_DEC_LIST_T();
		break;
		//FOLLOW
	case TOKEN_ID:
	case TOKEN_IF:
	case TOKEN_RETURN:
	case TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES:
		fprintf(xnxxout, "Rule ( VAR_DEC_LIST' ->epsilon)\n ");
		current_t = back_token();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//STMT_LIST -> STMT STMT_LIST_T
void parse_STMT_LIST() {

	int first[] = { TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_ID:
	case TOKEN_IF:
	case TOKEN_RETURN:
	case TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES:
		fprintf(xnxxout, "Rule ( STMT_LIST -> STMT STMT_LIST_T)\n ");
		current_t = back_token();
		parse_STMT();
		parse_STMT_LIST_T();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//STMT_LIST_T -> ; STMT STMT_LIST_T |epsilon
void parse_STMT_LIST_T() {

	int first[] = { TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES };
	current_t = next_token();

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
		fprintf(xnxxout, "Rule (STMT_LIST_T -> ; STMT STMT_LIST_T)\n ");
		
		parse_STMT();
		parse_STMT_LIST_T();
		break;
		//FOLLOW
	case TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
		fprintf(xnxxout, "Rule (STMT_LIST_T -> epsilon)\n ");
		current_t = back_token();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//STMT -> id STMT_T | COMP_STMT | IF_STMT | RETURN_STMT
void parse_STMT() {
	int first[] = { TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_ID:
		fprintf(xnxxout, "Rule (STMT ->  id STMT_T)\n");
		parse_STMT_T();
		break;

	case TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES:
		fprintf(xnxxout, "Rule (STMT ->  COMP_STMT)\n ");
		current_t = back_token();
		parse_COMP_STMT();
		break;
	case TOKEN_IF:
		fprintf(xnxxout, "Rule (STMT ->  IF_STMT)\n ");
		current_t = back_token();
		parse_IF_STMT();
		break;
	case TOKEN_RETURN:
		fprintf(xnxxout, "Rule (STMT -> RETURN_STMT)\n ");
		current_t = back_token();
		parse_RETURN_STMT();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//STMT_T -> VAR_T = EXPR | ( ARGS )
void parse_STMT_T() {

	int first[] = { TOKEN_ASSIGNMENT_OP_EQUAL,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES,TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_ASSIGNMENT_OP_EQUAL:
	case TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS:
		current_t = back_token();
		fprintf(xnxxout, "Rule (STMT_T -> VAR_T = EXPR )\n ");
		parse_VAR_T();
		match(TOKEN_ASSIGNMENT_OP_EQUAL);
		parse_EXPR();
		break;

	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule (STMT_T -> ( ARGS ))\n ");
		parse_ARGS();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES);
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//IF_STMT -> if ( CONDITION ) STMT
void parse_IF_STMT() {

	int first[] = { TOKEN_IF };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_IF:
		fprintf(xnxxout, "Rule (IF_STMT -> if ( CONDITION ) STMT)\n ");
		match(TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES);
		parse_CONDITION();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES);
		parse_STMT();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//ARGS -> ARG_LIST  |  epsilon
void parse_ARGS() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();

	switch (current_t->kind)
	{
	case  INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule (ARGS -> ARG_LIST)\n ");
		current_t = back_token();
		parse_ARG_LIST();
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
		fprintf(xnxxout, "Rule (ARGS -> epsilon)\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//ARG_LIST -> EXPR ARG_LIST_T
void parse_ARG_LIST() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();

	switch (current_t->kind)
	{
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule (ARG_LIST -> EXPR ARG_LIST_T)\n ");
		current_t = back_token();
		parse_EXPR();
		parse_ARG_LIST_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//ARG_LIST_T -> , EXPR ARG_LIST_T | epsilon
void parse_ARG_LIST_T() {

	int first[] = { TOKEN_SEPARACTION_SIGNS_COMMA };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();

	switch (current_t->kind)
	{
		//FIRST
	case TOKEN_SEPARACTION_SIGNS_COMMA:
		fprintf(xnxxout, "Rule (ARG_LIST_T -> , EXPR ARG_LIST_T)\n ");
		parse_EXPR();
		parse_ARG_LIST_T();
		break;
		//FOLLOW	
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
		fprintf(xnxxout, "Rule (ARG_LIST_T -> epsilon)\n ");
		current_t = back_token();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//RETURN_STMT -> return RETURN_STMT_T
void parse_RETURN_STMT() {

	int first[] = { TOKEN_RETURN };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES,TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_RETURN:
		fprintf(xnxxout, "Rule ( RETURN_STMT -> return RETURN_STMT_T )\n ");
		parse_RETURN_STMT_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//RETURN_STMT_T ->  EXPR | epsilon
void parse_RETURN_STMT_T() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES,TOKEN_SEPARACTION_SIGNS_SEMICOLON };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_ID:
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule ( RETURN_STMT_T -> EXPR )\n ");
		current_t = back_token();
		parse_EXPR();
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
		fprintf(xnxxout, "Rule ( RETURN_STMT_T -> epsilon )\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//VAR_T ->epsilon | [ EXPR_LIST ]
void parse_VAR_T() {

	int first[] = { TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_AR_OP_MULTI,TOKEN_AR_OP_ADD,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL,
		TOKEN_ASSIGNMENT_OP_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS:
		fprintf(xnxxout, "Rule (VAR_T -> [ EXPR_LIST ])\n ");
		parse_EXPR_LIST();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS);
		break;
	case  TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
	case TOKEN_SEPARACTION_SIGNS_COMMA:
	case TOKEN_AR_OP_MULTI:
	case TOKEN_AR_OP_ADD:
	case TOKEN_REL_OP_NOT_EQUAL:
	case TOKEN_REL_OP_EQUAL:
	case TOKEN_REL_OP_GREATER:
	case TOKEN_REL_OP_GREATER_EQUAL:
	case TOKEN_REL_OP_LESS:
	case TOKEN_REL_OP_LESS_EQUAL:
	case TOKEN_ASSIGNMENT_OP_EQUAL:
		fprintf(xnxxout, "Rule (VAR_T ->epsilon)\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}

//EXPR_LIST -> EXPR EXPR_LIST_T
void parse_EXPR_LIST() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS };
	current_t = next_token();

	switch (current_t->kind)
	{
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule ( EXPR_LIST -> EXPR EXPR_LIST_T )\n ");
		current_t = back_token();
		parse_EXPR();
		parse_EXPR_LIST_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//EXPR_LIST_T -> , EXPR EXPR_LIST_T |epsilon
void parse_EXPR_LIST_T() {

	int first[] = { TOKEN_SEPARACTION_SIGNS_COMMA };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_COMMA:
		fprintf(xnxxout, "Rule ( EXPR_LIST_T -> , EXPR EXPR_LIST_T )\n ");
		parse_EXPR();
		parse_EXPR_LIST_T();
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
		fprintf(xnxxout, "Rule ( EXPR_LIST_T -> epsilon )\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//CONDITION -> EXPR  rel_op  EXPR
void parse_CONDITION() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES };
	current_t = next_token();

	switch (current_t->kind)
	{
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule ( CONDITION -> EXPR  rel_op  EXPR )\n ");
		current_t = back_token();
		parse_EXPR();
		current_t = next_token();
		if ((current_t->kind == TOKEN_REL_OP_NOT_EQUAL) || (current_t->kind == TOKEN_REL_OP_EQUAL) || (current_t->kind == TOKEN_REL_OP_GREATER)
			|| (current_t->kind == TOKEN_REL_OP_GREATER_EQUAL) || (current_t->kind == TOKEN_REL_OP_LESS) || (current_t->kind == TOKEN_REL_OP_LESS_EQUAL))
		{

			parse_EXPR();
			break;
		}
		else
		{
			fprintf(xnxxout, "Expected token of type rel_op  at line : {%d},Actual token of type {%s}, token.lexeme :{%s}"
				, current_t->lineNumber, tokenStringCases(current_t->kind), current_t->lexeme);
			break;
		}

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}
}


//EXPR -> TERM EXPR_T
void parse_EXPR() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule ( EXPR -> TERM EXPR_T )\n ");
		current_t = back_token();
		parse_TERM();
		parse_EXPR_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));

		break;

	}
}


//EXPR_T -> + TERM EXPR_T | epsilon
void parse_EXPR_T() {

	int first[] = { TOKEN_AR_OP_ADD };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_AR_OP_ADD:
		fprintf(xnxxout, "Rule ( EXPR_T -> + TERM EXPR_T)\n ");
		parse_TERM();
		parse_EXPR_T();
		break;

	case TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
	case TOKEN_SEPARACTION_SIGNS_COMMA:
	case TOKEN_REL_OP_NOT_EQUAL:
	case TOKEN_REL_OP_EQUAL:
	case TOKEN_REL_OP_GREATER:
	case TOKEN_REL_OP_GREATER_EQUAL:
	case TOKEN_REL_OP_LESS:
	case TOKEN_REL_OP_LESS_EQUAL:
		fprintf(xnxxout, "Rule ( EXPR_T -> epsilon)\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//TERM -> FACTOR TERM_T
void parse_TERM() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_AR_OP_ADD,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case INT_NUM:
	case FLOAT_NUM:
	case TOKEN_ID:
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		fprintf(xnxxout, "Rule ( TERM -> FACTOR TERM_T )\n ");
		current_t = back_token();
		parse_FACTOR();
		parse_TERM_T();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//TERM_T -> * FACTOR TERM_T | epsilon
void parse_TERM_T() {

	int first[] = { TOKEN_AR_OP_MULTI };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_AR_OP_ADD,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_AR_OP_MULTI:
		fprintf(xnxxout, "Rule ( TERM_T -> * FACTOR TERM_T )\n ");
		parse_FACTOR();
		parse_TERM_T();
		break;
	case TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
	case TOKEN_SEPARACTION_SIGNS_COMMA:
	case TOKEN_REL_OP_NOT_EQUAL:
	case TOKEN_REL_OP_EQUAL:
	case TOKEN_REL_OP_GREATER:
	case TOKEN_REL_OP_GREATER_EQUAL:
	case TOKEN_REL_OP_LESS:
	case TOKEN_REL_OP_LESS_EQUAL:
	case TOKEN_AR_OP_ADD:
		fprintf(xnxxout, "Rule ( TERM_T -> epsilon )\n ");
		current_t = back_token();
		break;
	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


//FACTOR -> id FACTOR_T | int_num | float_num | ( EXPR )
void parse_FACTOR() {

	int first[] = { INT_NUM,FLOAT_NUM,TOKEN_ID,TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_AR_OP_MULTI,TOKEN_AR_OP_ADD,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_ID:
		fprintf(xnxxout, "Rule (FACTOR -> id FACTOR_T )\n");
		parse_FACTOR_T();
		break;
	case INT_NUM:
		fprintf(xnxxout, "Rule (FACTOR ->INT_NUM)\n");
		break;
	case FLOAT_NUM:
		fprintf(xnxxout, "Rule (FACTOR ->FLOAT_NUM)\n");
		break;
	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		
		fprintf(xnxxout, "Rule (FACTOR ->( EXPR ) )\n");
		parse_EXPR();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES);
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}

//FACTOR_T -> VAR_T | ( ARGS ) | epsilon
void parse_FACTOR_T() {

	int first[] = { TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES,TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS };
	int follow[] = { TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES, TOKEN_SEPARACTION_SIGNS_SEMICOLON,TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES
	,TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,TOKEN_SEPARACTION_SIGNS_COMMA,TOKEN_AR_OP_MULTI,TOKEN_AR_OP_ADD,TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,TOKEN_REL_OP_GREATER,TOKEN_REL_OP_GREATER_EQUAL,TOKEN_REL_OP_LESS,TOKEN_REL_OP_LESS_EQUAL };
	current_t = next_token();

	switch (current_t->kind)
	{
	case TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS:
		current_t = back_token();
		fprintf(xnxxout, "Rule (FACTOR_T -> VAR_T)\n ");
		parse_VAR_T();
		break;

	case TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES:
		
		fprintf(xnxxout, "Rule (FACTOR_T -> ( ARGS ) )\n ");
		parse_ARGS();
		match(TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES);
		break;

		//FOLLOW
	case TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES:
	case TOKEN_SEPARACTION_SIGNS_SEMICOLON:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES:
	case TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS:
	case TOKEN_SEPARACTION_SIGNS_COMMA:
	case TOKEN_REL_OP_NOT_EQUAL:
	case TOKEN_REL_OP_EQUAL:
	case TOKEN_REL_OP_GREATER:
	case TOKEN_REL_OP_GREATER_EQUAL:
	case TOKEN_REL_OP_LESS:
	case TOKEN_REL_OP_LESS_EQUAL:
	case TOKEN_AR_OP_ADD:
	case TOKEN_AR_OP_MULTI:
		fprintf(xnxxout, "Rule (FACTOR_T -> epsilon )\n ");
		current_t = back_token();
		break;

	default:
		tokenError(first, sizeof(first) / sizeof(first[0]), current_t->lineNumber, current_t->kind, current_t->lexeme);
		recovery(follow, sizeof(follow) / sizeof(follow[0]));
		break;
	}

}


void recovery(int* followArr, int sizeArr) {

	Token *token = back_token();
	int i, flagFollow = 0;

	while (!flagFollow) {
		token = next_token();
		if (token->kind == TOKEN_EOF) {
			exit(0);
		}
		else {
			for (i = 0; i < sizeArr; i++) {
				if (followArr[i] == token->kind) {
					flagFollow = 1;
					back_token();
					break;
				}
			}
		}
	}
}


