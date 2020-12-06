#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout,*xnxxout;
extern int yylex();

 


typedef enum eTOKENS
{
	/*NUMBERS*/
	INT_NUM,
	FLOAT_NUM,


	/*KEYWORDS*/
	TOKEN_IF,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_VOID,
	TOKEN_RETURN,


	/*AR_OP*/
	TOKEN_AR_OP_ADD,
	TOKEN_AR_OP_MULTI,

	/*REL_OP */

	TOKEN_REL_OP_NOT_EQUAL,
	TOKEN_REL_OP_EQUAL,
	TOKEN_REL_OP_GREATER,
	TOKEN_REL_OP_GREATER_EQUAL,
	TOKEN_REL_OP_LESS,
	TOKEN_REL_OP_LESS_EQUAL,


	/*ASSIGNMENT_OP */
	TOKEN_ASSIGNMENT_OP_EQUAL,

	/*SEPARACTION_SIGNS*/

	TOKEN_SEPARACTION_SIGNS_COMMA,
	TOKEN_SEPARACTION_SIGNS_COLON,
	TOKEN_SEPARACTION_SIGNS_SEMICOLON,
	TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES,
	TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES,
	TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS,
	TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS,
	TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES,
	TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES,

	/*comma   ,
	colon   :
	semicolon;
	parentheses()
	brackets[]
	curly braces     {   }
	*/


	TOKEN_ID,
	TOKEN_EOF

}eTOKENS;

/*Array of token strings*/


typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();
char *tokenStringCases(int kindCurrentToken);




void freeList();


#endif
