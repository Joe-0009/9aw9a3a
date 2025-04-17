#include "minishell.h"

static int compare_env_vars(const void *a, const void *b)
{
    return (ft_strcmp(*(char **)a, *(char **)b));
}

static void ft_swap_ptr(char **a, char **b)
{
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void ft_bubble_sort(char **arr, int n, int (*compar)(const void *, const void *))
{
    int i, j;
    i = 0;
    while (i < n - 1)
    {
        j = 0;
        while (j < n - i - 1)
        {
            if (compar(&arr[j], &arr[j + 1]) > 0)
                ft_swap_ptr(&arr[j], &arr[j + 1]);
            j++;
        }
        i++;
    }
}

static int print_env_vars(char **envp)
{
    int i, env_count;
    char **sorted_env;
    env_count = 0;
    while (envp[env_count])
        env_count++;
    sorted_env = malloc(sizeof(char *) * (env_count + 1));
    if (!sorted_env)
        return (1);
    i = -1;
    while (++i < env_count)
        sorted_env[i] = envp[i];
    sorted_env[i] = NULL;
    ft_bubble_sort(sorted_env, env_count, compare_env_vars);
    i = 0;
    while (sorted_env[i])
    {
        printf("declare -x %s\n", sorted_env[i]);
        i++;
    }
    free(sorted_env);
    return (0);
}

static int set_env_var(const char *var_name, const char *var_value)
{
    if (setenv(var_name, var_value, 1) != 0)
    {
        fprintf(stderr, "minishell: export: unable to set environment variable\n");
        return (1);
    }
    return (0);
}

static int is_valid_identifier(char *str)
{
    int i;
    if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
        return (0);
    i = 1;
    while (str[i])
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

int builtin_export(t_command *cmd, char **envp)
{
    char *equal_sign, *var_name, *var_value;
    int i, ret;
    if (cmd->args_count == 1)
        return (print_env_vars(envp));
    i = 1;
    ret = 0;
    while (i < cmd->args_count)
    {
        equal_sign = strchr(cmd->args[i], '=');
        if (equal_sign)
        {
            var_name = ft_substr(cmd->args[i], 0, equal_sign - cmd->args[i]);
            var_value = ft_strdup(equal_sign + 1);
            if (!var_name || !var_value)
            {
                fprintf(stderr, "minishell: export: memory allocation error\n");
                return (1);
            }
            if (!is_valid_identifier(var_name))
            {
                fprintf(stderr, "minishell: export: '%s': not a valid identifier\n", var_name);
                ret = 1;
            }
            else if (set_env_var(var_name, var_value) != 0)
                ret = 1;
            free(var_name);
            free(var_value);
        }
        else
        {
            if (!is_valid_identifier(cmd->args[i]))
            {
                fprintf(stderr, "minishell: export: '%s': not a valid identifier\n", cmd->args[i]);
                ret = 1;
            }
        }
        i++;
    }
    return (ret);
}

int builtin_unset(t_command *cmd)
{
    int i, ret;
    if (cmd->args_count == 1)
        return (0);
    i = 1;
    ret = 0;
    while (i < cmd->args_count)
    {
        if (!is_valid_identifier(cmd->args[i]))
        {
            fprintf(stderr, "minishell: unset: '%s': not a valid identifier\n", cmd->args[i]);
            ret = 1;
        }
        else if (unsetenv(cmd->args[i]) != 0)
        {
            fprintf(stderr, "minishell: unset: '%s': unable to unset var\n", cmd->args[i]);
            ret = 1;
        }
        i++;
    }
    return (ret);
}

int builtin_env(char **envp)
{
    int i = 0;
    while (envp[i] != NULL)
    {
        printf("%s\n", envp[i]);
        i++;
    }
    return (0);
}
