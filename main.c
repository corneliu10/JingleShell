#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "builtin.c"

#define TOK_BUF_SIZE 64
#define TOK_DELIM " \t\n\r\a"

pid_t childPid;

int launchCommand(char **args);

int logic_operator(char **args, int cnt_oper)
{
    int bufsize = TOK_BUF_SIZE, position = 0; 
    char **tokens = malloc(bufsize * sizeof(char *));
    
     if (!tokens)
    {
        fprintf(stderr, "jingleShell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; args[i]; i++)
    {
        if(strcmp(args[i],"&&") != 0 && strcmp(args[i],"||") != 0 && strcmp(args[i],";") != 0)
        {
            tokens[position] = args[i];
            position++;
        }
        else
        {
            launchCommand(tokens);
            for (int i = 0 ; i <= position; i++)
                tokens[i] = NULL;
            position = 0;
        }
    }

    return launchCommand(tokens);
}


char **splitLine(char *line)
{
    int bufsize = TOK_BUF_SIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "jingleShell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += TOK_BUF_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "jingleShell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

typedef struct command {
	int argc;
	char* args[100];
} Command;

void printCommands(Command *cmds) {
	for (int i = 0; i < 100; ++i) {
		if (cmds[i].argc == 0) break;

		for (int j = 0; j < cmds[i].argc; ++j)
			printf("%s ", cmds[i].args[j]);

		printf("\n");
	}
} 

Command* checkPipeLines(char **args, int *pipe_count) {
	Command *cmds = (Command *)malloc(100 * sizeof(Command));
	for (int i = 0; i < 100; ++i)
		for (int j = 0; j < 100; j++)
			cmds[i].args[j] = (char *)malloc(100 * sizeof(char));
	for (int i = 0; i < 100; ++i)
		cmds[i].argc = 0;

	for (int i = 0; args[i]; ++i) {
		if (strcmp(args[i], "|") == 0) {
			(*pipe_count)++;
			continue;
		}
		
		memcpy(cmds[*pipe_count].args[cmds[*pipe_count].argc++], args[i], sizeof(args[i]));
	}

	return cmds;
	//printCommands(cmds);
}

int launchCommand(char **args)
{
    int size = 1000,i,j;
    int pipe_count = 0;
    int fd[100][2],cid1,cid2,length,status;

    //char* string[100][100];// = {{"pwd", NULL} ,{"ls", "-la", NULL}, {"wc", "-l", NULL}}; 

    Command *commands = checkPipeLines(args, &pipe_count);

    if(pipe_count)
    {
        for(i = 0;i < pipe_count;i++)
        {
            pipe(fd[i]);
        }

        for(i = 0;i <= pipe_count;i++)
        {
            cid1 = fork();
            if(!cid1)
            {

                if(i!=0)
                {
                    dup2(fd[i-1][0], 0); // copy the input file descriptor
                }


                if(i!=pipe_count)
                {
                    dup2(fd[i][1], 1); // copy the output file descriptor
                }


                for(j = 0;j < pipe_count;j++)
                {   
                    close(fd[j][0]);
                    close(fd[j][1]);
                }

                execlp(commands[i].args[0], commands[i].args[0], NULL);
                //execvp(commands[i].args[0], (char *const char[])commands[i].args);
                exit(0);
            }
        }

        for(i = 0;i < pipe_count;i++)
        {
            close(fd[i][0]);
            close(fd[i][1]);
        }
        
        waitpid(cid1,&status,0);
    }
    else
    {
        int status;
	    pid_t pid, wpid;

	    pid = fork();
	    if (pid == 0)
	    {
	        // Child process
	        if (execvp(args[0], args) == -1)
	        {
	            perror("error on creating child process!");
	        }
	        exit(EXIT_FAILURE);
	    }
	    else if (pid < 0)
	    {
	        perror("error on forking!");
	    }
	    else
	    {
	        // Parent process
	        childPid = pid;
	        wpid = waitpid(pid, &status, WUNTRACED);
	    }
    }

    return 1;
}

int executeCommand(char **args)
{
    int i;
    int cnt_oper = 0;

    if (args[0] == NULL) {
        //printf("No command given!\n");
        return 1;
    }

    for(i = 0; args[i]; i++)
        if(strcmp(args[i], "&&") == 0 || strcmp(args[i], "||") == 0 || strcmp(args[i], ";") == 0)
            cnt_oper++;

    if (cnt_oper > 0)
    {
        logic_operator(args, cnt_oper);
        return 1;
    }

    for (int i=0; i < jshell_num_builtins(); ++i)
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    
    return launchCommand(args);
}

void sighandler(int sig_num) {
	//printf("Current Pid: %d Child Pid: %d\n", getpid(), childPid);
	signal(SIGTSTP, sighandler);
	if (childPid) {
		kill(childPid, SIGSTOP);
		printf("Stopped process with pid: %d\n", childPid);
	}
}

void runLoop()
{
    char *line;
    char **args;

    int status;

    signal(SIGTSTP, sighandler);
    do
    {
        line = readline(":> ");
        if(strlen(line) > 0)
            add_history(line);

        args = splitLine(line);
        status = executeCommand(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
	using_history();
    runLoop();

    return EXIT_SUCCESS;
}

