#include "minishell.h"

static int compare_env_vars(const void *a, const void *b)
{
    return (ft_strcmp(*(char **)a, *(char **)b));
}

static void ft_swap_ptr(char **a, char **b)
{
    char *tmp;

    tmp = *a;
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

int print_env_vars(char **envp)
{
    int i;
    int env_count;
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

int is_valid_identifier(char *str)
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
