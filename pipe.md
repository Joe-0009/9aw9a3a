```mermaid
flowchart TD
    A["main.c | shell_loop()"] --> B["process_command()"]
    B --> C["tokenize_input()"]
    B --> D["assign_token_types()"]
    B --> E["create_cmds()"]
    E --> F["expand_command_args()"]

    %% Variable expansion chain
    F --> G["expand_variables()"]
    G --> H["process_variable()"]
    H --> I["extract_var_name()"]
    H --> J["get_env_value()"]
    J --> K{Is $?}
    K -- Yes --> L["g_last_exit_status"]
    K -- No --> M["search envp"]

    %% Builtins and exec
    F --> N["executor.c | execute_command_list()"]
    N --> O["builtins.c | execute_builtin()"]
    N --> P["execute_single_command()"]

    %% Libft usage
    G --> Q["libft | ft_strdup, ft_strjoin, ft_itoa"]
    J --> Q