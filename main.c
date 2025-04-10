#include "minishell.h"


static void display_welcome(void)
{
    printf("\n");
    printf(" ╔════════════════════════════════════╗\n");
    printf(" ║            MINISHELL               ║\n");
    printf(" ╚════════════════════════════════════╝\n");
    printf(" Welcome to Minishell! Type 'exit' to quit.\n\n");
}

void print_Cmd_list(t_command *cmd)
{
    int i = 0;
    while (cmd)
    {
        i = 0;
        while (i < cmd->args_count)
            printf("%s ", cmd->args[i++]);
        printf("\n");
        cmd = cmd->next;
    }
}
static int process_command(char *input, char **envp)
{
    t_token *tokens;
    t_command *cmds;
    int status;

    tokens = NULL;
    cmds = NULL;
	status = 0;
    if (ft_strcmp(input, "exit") == 0)
        return 1;
    tokens = tokenize_input(input);
    if (!tokens)
        return 0;
    assign_token_types(tokens);
    cmds = create_cmds(&tokens);
    if (cmds)
    {

        status = execute_command_list(cmds, envp);
        printf("Command exit status: %d\n", status);
        free_command_list(cmds);
    }
    else if (tokens)
    {
        fprintf(stderr, "minishell: syntax error in command\n");
        ft_token_clear(&tokens, free);
    }
    return 0;
}



static void shell_loop(char **envp)
{
    char *input;
    int should_exit;

    should_exit = 0;
    while (!should_exit)
    {
        input = readline("minishell> ");
        if (input == NULL)
        {
            printf("exit\n");
            break;
        }
        if (input[0] != '\0')
        {
            add_history(input);
            should_exit = process_command(input, envp);
        }
        free(input);
    }
}


int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;

    setup_signals();
    display_welcome();
    shell_loop(envp);
    return 0;
}