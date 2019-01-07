#include <stdio.h>
#include <unistd.h>

/*
    Function Declarations for builtin shell commands:
*/

int jshell_cd(char **args);
int jshell_print(char **args);
int jshell_exit(char **args);

/*
  List of builtin commands
 */

char *builtin_str[] = {
    "cd",
    "print",
    "exit"};

int (*builtin_func[])(char **) = {
    &jshell_cd,
    &jshell_print,
    &jshell_exit};

int jshell_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

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

int jshell_exit(char **args)
{
    return 0;
}