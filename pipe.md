```mermaid
flowchart TB
    A[main.c] --> B[tokenize_input() <br> assign_token_types() <br> create_cmds()]
    A --> C[executor.c <br> execute_command_list()]
    A --> D[builtins.c]

    C --> D
    C --> E[libft <br> ft_split.c, ft_strjoin.c, ft_lstnew.c, ft_lstmap.c]
    D --> E