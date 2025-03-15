```mermaid
flowchart TD
    A[Input String] --> B[expand_variables]
    B --> C{Process character by character}
    C -->|Character is $| D{Check state and next char}
    D -->|In single quotes or end of string| J[Add $ literally]
    D -->|Valid variable character follows| E[process_variable]
    E --> F[extract_var_name]
    F --> G[get_env_value]
    G --> H[Add value to result]
    C -->|Other character| I[update_quote_state]
    I --> J[Add character to result]
    J --> C
    C -->|End of string| K[Return expanded string]
    
    L[expand_command_args] --> M{Process each argument}
    M --> N[Expand variables in argument]
    M --> O[Process redirections]
    O --> P{Not a heredoc?}
    P -->|Yes| Q[Expand variables in file name]