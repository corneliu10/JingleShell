#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

/*
    Function Declarations for builtin shell commands:
*/

int jshell_cd(char **args);
int jshell_print(char **args);
int jshell_history(char **args);
int jshell_fg(char **args);
int jshell_help(char **args);
int jshell_exit(char **args);

/*
  List of builtin commands
 */

char *builtin_str[] = {
    "history",
    "cd",
    "print",
    "fg",
    "help",
    "exit" };

int (*builtin_func[])(char **) = {
    &jshell_history,
    &jshell_cd,
    &jshell_print,
    &jshell_fg,
    &jshell_help,
    &jshell_exit};

int jshell_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

int jshell_history(char **args)
{
	/* get the state of your history list (offset, length, size) */
    HISTORY_STATE *myhist = history_get_history_state();
    int length = myhist->length;

    /* retrieve the history list */
    HIST_ENTRY **mylist = history_list();

    if(args[1] != NULL && strcmp(args[1],"-c") == 0)
    {
        clear_history();
        return 1;
    }

    if (args[1] != NULL && length > atoi(args[1])) 
        length = atoi(args[1]);

    printf("\n");
    for (int i = myhist->length - length; i < myhist->length; i++)  /* output history list */
        printf("%d. %s  %s\n", i+1, mylist[i]->line, mylist[i]->timestamp);
    printf("\n");

    free (myhist);  /* free HISTORY_STATE */
	
    return 1;
}

int jshell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "expected argument! \n");
    }
    else if (chdir(args[1]) != 0)
    {
        perror("error on changing directory!\n");
    }
    return 1;
}

int jshell_print(char **args)
{
    int a = 10, b;

    while(1);
    for (b = 1; b <= a; b++)
    {
        for (int x = a - 1; x >= b; x--)
        {
            printf(" ");
        }

        for (int x = 1; x <= b; x++)
        {
            printf("* ");
        }

        printf("\n");
    }

    for (int i = 0; i <= 1; ++i) {
        for (b = 0; b <= a / 2 + a / 3 + 1; ++b)
            if (b > a / 2)
                printf("* ");
            else
                printf(" ");
        
        printf("\n");
    }
    printf("\n");
    
    printf("*** MERRY CHRISTMAS ***\n\n");

    printf("\n");
    return 1;
}

int jshell_fg(char **args)
{
    //printf("%s %s", args[0], args[1]);
    kill(getpid(), SIGCONT);
}

int jshell_help(char **args)
{
    int i;
    printf("Jingle Shell\n");
    printf("The following are built in:\n");

    for (i = 0; i < jshell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("The pipeline is working!\n");
    printf("Use the man command for information on other programs.\n");
    return 1;
}

int jshell_exit(char **args)
{
    return 0;
}
