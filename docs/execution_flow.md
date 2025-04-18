# Minishell Execution Flow

This diagram illustrates the full execution process of commands in minishell, from parsing to command execution, including the specific function names from the codebase.

```mermaid
flowchart TD
    %% Main flow
    Start([User Input]) --> Parse["parse_input()"]
    Parse --> TokenList["create_token_list()"]
    TokenList --> CommandList["build_command_list()"]
    CommandList --> CheckCmd{"execute_command_list()\nCommand Type?"}
    
    %% Command type decision
    CheckCmd -- "Single Parent Builtin\n(cd, export, unset, exit)" --> ParentBuiltin["execute_builtin()"]
    CheckCmd -- "Pipeline or\nOther Commands" --> SetupHeredocs["setup_all_heredocs()"]
    
    %% Heredoc handling
    SetupHeredocs -- Success --> CommandLoop["Command Loop"]
    SetupHeredocs -- Failure --> ReturnError["Return Error (1)"]
    
    %% Command processing loop
    CommandLoop --> SetupPipe["setup_command_pipe()"]
    SetupPipe --> ExpandVars["expand_command_args()"]
    ExpandVars --> ForkProcess["fork()"]
    ForkProcess --> ProcessType{"Process Type?"}
    
    %% Child process flow
    ProcessType -- Child --> SetupRedirs["child_process()\napply_redirections()"]
    SetupRedirs --> CommandCheck{"execute_single_command()\nCommand Type?"}
    CommandCheck -- Builtin --> ExecBuiltin["handle_builtin_command()"]
    CommandCheck -- External --> FindPath["handle_external_command()\nget_exec_path()"]
    FindPath --> CheckPath{"Path Found?"}
    CheckPath -- Yes --> Execve["execve()"]
    CheckPath -- No --> CommandNotFound["Command Not Found"]
    CommandNotFound --> ChildExit["exit(127)"]
    Execve -- Failure --> ExecError["perror()"]
    ExecError --> ChildExit
    ExecBuiltin --> ChildExit
    
    %% Parent process flow
    ProcessType -- Parent --> CloseHeredocs["close_heredoc_fds()"]
    CloseHeredocs --> ManagePipes["parent_process()"]
    ManagePipes --> NextCommand{"More Commands?"}
    NextCommand -- Yes --> CommandLoop
    NextCommand -- No --> WaitChildren["wait_for_children()"]
    WaitChildren --> CollectStatus["Collect Exit Status"]
    CollectStatus --> End([End Execution])
    
    %% Connect parent builtin to end
    ParentBuiltin --> End
    
    %% Additional information
    subgraph "Environment Handling"
        ExpandVars --> ConvertEnv["env_list_to_envp()"]
        ConvertEnv --> FreeTempEnv["safe_doube_star_free()"]
    end
    
    subgraph "Redirection Handling"
        SetupRedirs --> InRedir["handle_input_redir()"]
        SetupRedirs --> OutRedir["handle_output_redir()"]
        SetupRedirs --> HereDoc["handle_heredoc_redir()"]
        SetupRedirs --> Append["handle_append_redir()"]
    end
    
    subgraph "Path Resolution"
        FindPath --> CheckSlash["find_executable_path()"]
        CheckSlash -- Contains '/' --> UseAsPath["Use as Direct Path"]
        CheckSlash -- No '/' --> GetPATH["get_path_value()"]
        GetPATH --> TryPaths["try_path_directories()"]
        TryPaths --> CheckFound{"Found?"}
        CheckFound -- Yes --> ReturnPath["Return Full Path"]
        CheckFound -- No --> ReturnNull["Return Original (ft_strdup)"]
    end
    
    %% Memory Management
    subgraph "Memory Management"
        ConvertEnv --> CallCallocArray["ft_calloc()"]
        FindPath --> StrdupPaths["ft_strdup()"]
        CommandLoop --> FreeEnvp["safe_doube_star_free()"]
        ChildExit --> FreeExecResources["safe_free()"]
        End --> FinalCleanup["cleanup_resources()"]
    end
```

## Key Functions in Execution Flow

1. **Command Parsing & Setup**
   - `execute_command_list()`: Main function coordinating command execution
   - `setup_all_heredocs()`: Prepares all heredoc redirections before execution
   - `setup_command_pipe()`: Sets up pipes between commands

2. **Command Execution**
   - `child_process()`: Handles execution in child processes
   - `handle_builtin_command()`: Executes shell builtin commands
   - `handle_external_command()`: Handles execution of external programs
   - `execute_single_command()`: Routes commands to builtin or external handler

3. **Path Resolution**
   - `get_exec_path()`: Gets executable path using temp environment
   - `find_executable_path()`: Searches for command in PATH directories 

4. **Environment Management**
   - `env_list_to_envp()`: Converts internal environment to execve format
   - `expand_command_args()`: Expands environment variables in commands

5. **Process & Pipe Management**
   - `parent_process()`: Manages pipes in parent shell
   - `wait_for_children()`: Waits for all child processes and collects status

6. **Memory Management**
   - `safe_doube_star_free()`: Safely frees string arrays
   - `safe_free()`: Safely frees individual pointers