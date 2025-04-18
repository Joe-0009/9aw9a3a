# Command Evaluation Flow

This diagram shows how minishell evaluates and decides how to execute different types of commands.

```mermaid
flowchart TD
    %% Main Input Flow
    Start([User Input]) --> Parse["parse_input(input_line)"]
    Parse --> TokenList["create_token_list(tokens)"]
    TokenList --> CommandList["build_command_list(token_list)"]
    CommandList --> CheckCmd{"execute_command_list(cmd_list, env_list)"}
    
    %% Command type decision with details
    CheckCmd -- "Single Parent Builtin\n(cd, export, unset, exit)" --> PrepBuiltin["expand_command_args(cmd_list, envp)"]
    PrepBuiltin --> ParentBuiltin["execute_builtin(cmd, env_list)"]
    
    CheckCmd -- "Pipeline or\nother commands" --> SetupHeredocs["setup_all_heredocs(cmd_list)"]
    
    %% Heredoc handling with details
    SetupHeredocs -- "Success (0)" --> CommandLoop["Loop through commands"]
    SetupHeredocs -- "Failure (-1)" --> ReturnError["Return Error Code 1"]
    
    %% Return paths
    ParentBuiltin --> End([End Execution])
    
    %% Link to next stage
    CommandLoop --> NextPart([Continue to Pipeline Processing])
    
    %% Builtins detail subgraph
    subgraph "Parent Builtin Details"
        ParentBuiltin --> CD["cd: Change directory"]
        ParentBuiltin --> Export["export: Set env variables"] 
        ParentBuiltin --> Unset["unset: Remove env variables"]
        ParentBuiltin --> Exit["exit: Terminate shell"]
    end
```

## Key Functions Details

- `execute_command_list(t_command *cmd_list, t_env **env_list)`: 
  - Evaluates command type: single builtin vs pipeline
  - For parent builtins: expands variables then runs directly
  - For pipelines: sets up heredocs, pipes, handles processes
  - Returns: status code from command execution (0-255)

- `is_parent_builtin(char *cmd)`: 
  - Checks commands that must run in parent process
  - Returns: 1 for "cd", "export", "unset", "exit"; 0 otherwise
  - Implementation: Simple string comparison with ft_strcmp

- `execute_builtin(t_command *cmd, t_env **env_list)`:
  - Routes to specific builtin function based on command name
  - Handles: cd, echo, pwd, export, unset, env, exit
  - Returns: Exit status from builtin (typically 0 for success)

- `setup_all_heredocs(t_command *cmd_list)`:
  - Creates temp files for all heredoc redirections
  - Processes: <<EOF type redirections
  - Returns: -1 on error, 0 for success