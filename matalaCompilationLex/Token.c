#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
Node* currentNode = NULL;


#define TOKEN_ARRAY_SIZE 1000





/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{

	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray =
			(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray =
				(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token() {

	if (currentNode == NULL) {
		return NULL;
	}

	if (currentIndex == 0) {
		if (currentNode->prev == NULL) { 
			currentIndex--;
		}
		else {
			currentNode = currentNode->prev;
			currentIndex = TOKEN_ARRAY_SIZE - 1;
		}
	}
	else
		currentIndex--;

	return &currentNode->tokensArray[currentIndex];
}


/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/


Token *next_token() {

	if (currentNode == NULL) { 
		yylex();
	}

	else if (currentIndex == TOKEN_ARRAY_SIZE - 1) { 
		if (currentNode->next == NULL) { 
			yylex();
		}
		else {
			currentIndex = 0;
			currentNode = currentNode->next;
		}
	}
	else {
		if (currentNode->tokensArray[currentIndex + 1].lexeme == NULL) { 
			yylex();
		}
		else  currentIndex++;
	}
	return &currentNode->tokensArray[currentIndex];
}



char *tokenStringCases(int kindCurrentToken) { 

	switch (kindCurrentToken) {
	case	INT_NUM: return "'{INT_NUM}'";
	case	FLOAT_NUM: return "'{FLOAT_NUM}'";
	case	TOKEN_IF: return "'{TOKEN_IF}'";
	case    TOKEN_INT: return "'{TOKEN_INT}'";
	case	TOKEN_FLOAT: return "'{TOKEN_FLOAT}'";
	case    TOKEN_VOID: return "'{TOKEN_VOID}'";
	case	TOKEN_RETURN: return "'{TOKEN_RETURN}'";
	case	TOKEN_AR_OP_ADD: return "'{TOKEN_AR_OP_ADD}'";
	case	TOKEN_AR_OP_MULTI: return "'{TOKEN_AR_OP_MULTI}'";
	case	TOKEN_REL_OP_NOT_EQUAL: return "'{TOKEN_REL_OP_NOT_EQUAL}'";
	case	TOKEN_REL_OP_EQUAL: return "'{TOKEN_REL_OP_EQUAL}'";
	case	TOKEN_REL_OP_GREATER: return "'{TOKEN_REL_OP_GREATER}'";
	case	TOKEN_REL_OP_GREATER_EQUAL: return "'{TOKEN_REL_OP_GREATER_EQUAL}'";
	case	TOKEN_REL_OP_LESS: return "'{TOKEN_REL_OP_LESS}'";
	case	TOKEN_REL_OP_LESS_EQUAL: return "'{TOKEN_REL_OP_LESS_EQUAL}'";
	case	TOKEN_ASSIGNMENT_OP_EQUAL: return "'{TOKEN_ASSIGNMENT_OP_EQUAL}'";
	case	TOKEN_SEPARACTION_SIGNS_COMMA: return "'{TOKEN_SEPARACTION_SIGNS_COMMA}'";
	case	TOKEN_SEPARACTION_SIGNS_COLON: return "'{TOKEN_SEPARACTION_SIGNS_COLON}'";
	case	TOKEN_SEPARACTION_SIGNS_SEMICOLON: return "'{TOKEN_SEPARACTION_SIGNS_SEMICOLON}'";
	case	TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES: return "'{TOKEN_SEPARACTION_SIGNS_OPEN_PARENTHESES}'";
	case	TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES: return "'{TOKEN_SEPARACTION_SIGNS_CLOSE_PARENTHESES}'";
	case	TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS: return "'{TOKEN_SEPARACTION_SIGNS_OPEN_BRACKETS}'";
	case	TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS: return "'{TOKEN_SEPARACTION_SIGNS_CLOSE_BRACKETS}'";
	case	TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES: return "'{TOKEN_SEPARACTION_SIGNS_OPEN_CURLY_BRACES}'";
	case	TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES: return "'{TOKEN_SEPARACTION_SIGNS_CLOSE_CURLY_BRACES}'";
	case	TOKEN_ID: return "'{TOKEN_ID}'";
	case	TOKEN_EOF: return "'{TOKEN_EOF}'";

	default:
		return "ERROR!";
	}
}

void freeList() {
	while (currentNode->prev != NULL) {
		currentNode = currentNode->prev;
		free(currentNode->next->tokensArray);
		free(currentNode->next);
	}
	free(currentNode->tokensArray);
	free(currentNode);
	currentIndex = 1;
	currentNode = NULL;
}
