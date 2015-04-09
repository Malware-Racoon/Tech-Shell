#ifndef PARSE_H
#define PARSE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> //C99 bool type

#define CMD_BUFFER 20 //20 commands per input
#define TOKEN_BUFFER 20 //20 tokens per command
#define TOKEN_LENGTH 80 //80 char per token

#define PARSE_CMD_OVERFLOW_ERR 1
#define PARSE_TOK_OVERFLOW_ERR 2
#define PARSE_TOK_LEN_OVERFLOW_ERR 3
#define PARSE_STRING_CLOSE_ERR 4
#define REDIRECT_INPUT 1
#define REDIRECT_OUTPUT 2

struct Command {
	char tokens[TOKEN_BUFFER][TOKEN_LENGTH];
	bool pipes;
	int  num_pipes;
	int redirect;
	char file[TOKEN_LENGTH];
	bool fork;
};
int parse_input(struct Command *cmd, int command_buffer, int token_buffer, int token_length);
bool has_command(struct Command cmd);
bool has_token(char* token);
#endif

struct Command cmds[CMD_BUFFER]; //Store each Cmd