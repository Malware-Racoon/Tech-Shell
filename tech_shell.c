#include "parse.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i;
	int j;
	int k;
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	char *path = getenv("PATH");
	int status;
	pid_t pid;
	pid_t pid2;
	int fd;
	fpos_t pos;
	int fds[2];
	struct Variable 
	{
		char name[TOKEN_LENGTH];
		char value[TOKEN_LENGTH];
	};

struct Variable var[1024];
int count = 0;

	while(true)
	{
		//clear Command Struct array
		memset(&cmds, 0, sizeof(struct Command)*CMD_BUFFER);
		//Prompt User
		//printf("%% ");
		printf("Please enter a command: ");

		int success = parse_input(cmds, CMD_BUFFER, TOKEN_BUFFER, TOKEN_LENGTH);
		//Get input and parse into struct array
		if(success == 0)
		{
			int num_cmds = 0;
			int num_tokens = 0;
			for(i = 0; i < CMD_BUFFER; i++)
			{
				if(has_command(cmds[i])) //Command contains at least one token
				{
					//for(j = 0; j < CMD_BUFFER; j++)
					{
						if(has_token(cmds[i].tokens[j]))
						{
							if(cmds[i].redirect == REDIRECT_OUTPUT)
							{
								fflush(stdout);
						 		fgetpos(stdout, &pos);
						 		fd = dup(fileno(stdout));
						 		freopen(cmds[i].file, "w", stdout);
							}
							else if(cmds[i].redirect == REDIRECT_INPUT) 
							{
								printf("%i\n",cmds[i].pipes);
								fd = dup(fileno(stdin));
								freopen(cmds[i].file, "r", stdin);
							}
							else if(cmds[i].pipes == true)
							{
								char *temp = ".temp";
								char *tokens[TOKEN_BUFFER+1];
									for(k = 0; k < TOKEN_BUFFER; k++) 
									{
										if(has_token(cmds[i].tokens[k])) 
										{
											tokens[k] = &cmds[i].tokens[k][0];
										} else {
											tokens[k] = NULL;
										}
									}
								char *tokens2[TOKEN_BUFFER+1];
									for(k = 0; k < TOKEN_BUFFER; k++) 
									{
										if(has_token(cmds[i+1].tokens[k])) {
											tokens2[k] = &cmds[i].tokens[k][0];
										} else {
											tokens2[k] = NULL;
										}
									}
								fflush(stdout);
						 		fgetpos(stdout, &pos);
						 		fd = dup(fileno(stdout));
						 		freopen(temp, "w", stdout);
						 		if((pid=fork()) == 0)
						 		{
							 		execvp(cmds[i].tokens[0],tokens);
								}
								while(wait(&status) != pid);
								fflush(stdout);
								dup2(fd, fileno(stdout));
								close(fd);
								clearerr(stdout);
								fsetpos(stdout, &pos);
								fd = dup(fileno(stdin));
								freopen(temp, "r", stdin);
								if((pid2=fork()) == 0)
								{
									execvp(cmds[i+1].tokens[0],tokens2);
								}
								while(wait(&status) != pid2);
								dup2(fd, fileno(stdin));
								close(fd);
								break;
							}
							if(strcmp(cmds[i].tokens[0],"cd") == 0)
							{
								if(chdir(cmds[i].tokens[1])==-1)
								{
									printf("%s is not a valid directory\n",cmds[i].tokens[1]);
								}
								getcwd(cwd, sizeof(cwd));
								break;
							}
							else if(strcmp(cmds[i].tokens[0],"pwd") == 0)
							{
								printf("current working directory is: %s\n", cwd);
								break;
							}
							else if(strcmp(cmds[i].tokens[0],"exit") == 0)
							{
								return(0);
							}
							else if(strcmp(cmds[i].tokens[0],"set") == 0)
							{
								int a;
								int d;
								int e;
								for(e=0;e <= count; e++)
								{
									if(e == count)
									{
										for (a=0;cmds[i].tokens[1][a] != '\0'; a++)
										{
											var[count].name[a]= cmds[i].tokens[1][a];
										}

										for (a=0;cmds[i].tokens[2][a] != '\0'; a++)
										{
											if(cmds[i].tokens[2][a] != '"')
											{
												var[count].value[a-1] = cmds[i].tokens[2][a];
											}	
										}
										count++;
										break;
									}
									else if(!strcmp(cmds[i].tokens[j+1],var[e].name))
									{
										for (d=0;cmds[i].tokens[2][d] != '\0'; d++)
										{
											if(cmds[i].tokens[2][d] != '"')
											{
												var[e].value[d-1] = cmds[i].tokens[2][d];
											}	
										}
										var[e].value[d-2] = cmds[i].tokens[2][d];
										break;
									}
								}
								break;
							}
							else if(strcmp(cmds[i].tokens[j],"list") == 0)
							{
								int b;
								for (b=0; b < count; b++)
								{
									printf("%s: %s\n",var[b].name,var[b].value);

								}

							}
							else
							{
								if (fork()== 0) 
								{ // the child process
									char *tokens[TOKEN_BUFFER+1];
									for(k = 0; k < TOKEN_BUFFER; k++) 
									{
										if(has_token(cmds[i].tokens[k])) {
											tokens[k] = &cmds[i].tokens[k][0];
										} else {
											tokens[k] = NULL;
										}
									}
									if(execvp(cmds[i].tokens[0],tokens) < 0)
										{
											printf("%s is not an vaild comand\n", cmds[i].tokens[0]);
											exit(1);
										}
			    				} 
    							else 
    							{ // the parent process
        							pid = wait(&status);  
    							} 
							}	
						} 
						else 
						{
							break; //last token
						}
					}
					if(cmds[i].redirect == REDIRECT_INPUT) {
						dup2(fd, fileno(stdin));
						close(fd);
					} else if(cmds[i].redirect == REDIRECT_OUTPUT) {
					 	fflush(stdout);
						dup2(fd, fileno(stdout));
						close(fd);
						clearerr(stdout);
						fsetpos(stdout, &pos);	
					}
					
				}
				else
				{
					num_cmds = i; //last command
					break;
				}
			}
		}	
	}	
}

