#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.c"

#define TOK_BUF_SIZE 64
#define TOK_DELIM " \t\n\r\a"

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

int launchCommand(char **args)
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
        wpid = waitpid(pid, &status, WUNTRACED);
    }

    return 1;
}

int executeCommand(char **args)
{
    int i;

    if (args[0] == NULL) {
        //printf("No command given!\n");
        return 1;
    }

    for (int i=0; i < jshell_num_builtins(); ++i)
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    
    return launchCommand(args);
}

void runLoop()
{
    char *line;
    char **args;

    int status;

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

