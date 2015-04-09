#include "parse.h"
// Parses line from Stdin into cmd struct array containing an array of the tokens,
// and weather the command pipes into the next command
int parse_input(struct Command *cmd, int command_buffer, int token_buffer, int token_length)
{
	int cmdin_buffer = 40;
	char cmdin[cmdin_buffer];
	
	//clear Command Input array
	memset(cmdin, 0, sizeof(char)*cmdin_buffer);
	fgets(cmdin, cmdin_buffer, stdin);

	int cmd_counter = 0; //current command (i)
	int cmd_token = 0; //current token in command (j)
	int cmd_token_pos = 0; //current string position in token (k)
	
	bool in_string = false; //For String Literal Error/Edge Case handling
	int i;
	for(i = 0; true; i++) //parse indivdual commands and tokens, and label pipes
	{
		//trims all whitespace before beginning of cmd
		while(cmd_token_pos == 0 && (cmdin[i] == ' ' || cmdin[i] == '	'))
			i++;
		char c = cmdin[i];
		if(c == '\n') //new line, end of input
			break;
		else if(c == '\0') //null, we probably have more to get
		{
			fgets(cmdin, cmdin_buffer, stdin);
			i = -1; //will incrament and start at zero
			continue;
		}
		else if(in_string || c == '\"') //handle string notation errors
		{
			if(cmd_token_pos != 0) 
			{
				if(cmd_token == token_buffer-1)
					return(PARSE_TOK_OVERFLOW_ERR);
				cmd_token++;
				cmd_token_pos = 0;
			}
			do {
				if(cmdin[i+1] == '\0')
				{
					in_string = true;
					continue;
				}
				if(cmdin[i+1] == '\n')
					return(PARSE_STRING_CLOSE_ERR);
				if(cmd_token_pos == token_length)
					return(PARSE_TOK_LEN_OVERFLOW_ERR);
				cmd[cmd_counter].tokens[cmd_token][cmd_token_pos++] = cmdin[i];
				i++;
			} while (cmdin[i] != '\"');
			cmd[cmd_counter].tokens[cmd_token][cmd_token_pos++] = cmdin[i];
			if(cmd_token == token_buffer-1)
				return(PARSE_TOK_OVERFLOW_ERR);
			cmd_token++;
			cmd_token_pos = 0;
			in_string = false;
		}
		bool cont = true;
		int file_pos = 0;
		switch(c)
		{
			case '\"':
				break;
			case '|': //set pipe then treat as moving to next command
				cmd[cmd_counter].pipes = true; //This command pipes to the next
			case ';': //end of a command
				if(cmd_counter == command_buffer-1)
					return(PARSE_CMD_OVERFLOW_ERR);
				cmd_counter++;
				cmd_token = 0;
				cmd_token_pos = 0;
				break;
			case '<': //Seperate Special Characters
				cmd[cmd_counter].redirect = REDIRECT_INPUT;
				i++;
				while(cmdin[i] == ' ' || cmdin[i] == '	') i++; //remove whitespace
				while(cmdin[i] != ' ' && cmdin[i] != '	') 
				{
					cmd[cmd_counter].file[file_pos++] = cmdin[i]; //save redirect file
					if(cmdin[i+1] == '\n' || cmdin[i+1] == '\0') 
					{
						break;
					}
					i++;
				}
				while(cmdin[i+1] == ' ' || cmdin[i+1] == '	') i++; //remove whitespace
				if(cmd_token_pos != 0)
				{
					if(cmd_token == token_buffer-1)
						return(PARSE_TOK_OVERFLOW_ERR);
					cmd_token++;
					cmd_token_pos = 0;
				}
				break;
			case '>':
				cmd[cmd_counter].redirect = REDIRECT_OUTPUT;
				i++;
				while(cmdin[i] == ' ' || cmdin[i] == '	') i++; //remove whitespace
				while(cmdin[i] != ' ' && cmdin[i] != '	') {
					cmd[cmd_counter].file[file_pos++] = cmdin[i]; //save redirect file
					if(cmdin[i+1] == '\n' || cmdin[i+1] == '\0') {
						break;
					}
					i++;
				}
				while(cmdin[i] == ' ' || cmdin[i] == '	') i++; //remove whitespace
				if(cmd_token_pos != 0)
				{
					if(cmd_token == token_buffer-1)
						return(PARSE_TOK_OVERFLOW_ERR);
					cmd_token++;
					cmd_token_pos = 0;
				}
				break;
			case '&':
				cmd[cmd_token].fork = true;
				if(cmd_token == token_buffer-1)
					return(PARSE_TOK_OVERFLOW_ERR);
				if(cmd_token_pos != 0) {
					cmd_token++;
					cmd_token_pos = 0;
				}
				cmd[cmd_counter].tokens[cmd_token][cmd_token_pos++] = c;
			case ' ': //treat all whitespace delimiters the same
			case '	':
				if(cmd_token == token_buffer-1)
					return(PARSE_TOK_OVERFLOW_ERR);
				cmd_token++;
				cmd_token_pos = 0;
				while(cmdin[i+1] == ' ' || cmdin[i+1] == '	') i++; //remove remaining before next token
				break;
			default:
				if(cmd_token_pos == token_length) 
					return(PARSE_TOK_LEN_OVERFLOW_ERR);
				cmd[cmd_counter].tokens[cmd_token][cmd_token_pos++] = c;
				break;
		}
	}
	return(0); //success!
}

bool has_command(struct Command cmd)
{
	return(has_token(cmd.tokens[0])); //at least one token
}

bool has_token(char* token)
{
	return(token[0] != '\0'); //at least one char
}
