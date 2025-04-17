#include "minishell.h"

int builtin_export(t_command *cmd, char **envp)
{
    
}

int builtin_unset(t_command *cmd)
{
  
}

int builtin_env(char **envp)
{
    int i;
    
    i = 0;
    while (envp[i] != NULL)
    {
        printf("%s\n", envp[i]);
        i++;
    }
    return (0);
}