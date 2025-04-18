# Minishell Execution Flow

This diagram illustrates the full execution process of commands in minishell, from parsing to command execution.

```mermaid
flowchart TD
    %% Main flow
    Start([User Input]) --> Parse[Parse Input]
    Parse --> TokenList[Create Token List]
    TokenList --> CommandList[Build Command List]
    CommandList --> CheckCmd{Command Type?}
    
    %% Command type decision
    CheckCmd -- "Single Parent Builtin\n(cd, export, unset, exit)" --> ParentBuiltin[Execute in Parent Process]
    CheckCmd -- "Pipeline or\nOther Commands" --> SetupHeredocs[Setup All Heredocs]
    
    %% Heredoc handling
    SetupHeredocs -- Success --> CommandLoop[Process Each Command]
    SetupHeredocs -- Failure --> ReturnError[Return Error]
    
    %% Command processing loop
    CommandLoop --> SetupPipe[Setup Pipes]
    SetupPipe --> ExpandVars[Expand Environment Variables]
    ExpandVars --> ForkProcess[Fork Process]
    ForkProcess --> ProcessType{Process Type?}
    
    %% Child process flow
    ProcessType -- Child --> SetupRedirs[Setup Redirections]
    SetupRedirs --> CommandCheck{Command Type?}
    CommandCheck -- Builtin --> ExecBuiltin[Execute Builtin]
    CommandCheck -- External --> FindPath[Find Executable Path]
    FindPath --> CheckPath{Path Found?}
    CheckPath -- Yes --> Execve[Execute with execve]
    CheckPath -- No --> CommandNotFound[Command Not Found]
    CommandNotFound --> ChildExit[Exit Child with Status 127]
    Execve -- Failure --> ExecError[Print Error]
    ExecError --> ChildExit
    ExecBuiltin --> ChildExit
    
    %% Parent process flow
    ProcessType -- Parent --> CloseHeredocs[Close Heredoc FDs]
    CloseHeredocs --> ManagePipes[Manage Pipe FDs]
    ManagePipes --> NextCommand{More Commands?}
    NextCommand -- Yes --> CommandLoop
    NextCommand -- No --> WaitChildren[Wait for Children]
    WaitChildren --> CollectStatus[Collect Exit Status]
    CollectStatus --> End([End Execution])
    
    %% Connect parent builtin to end
    ParentBuiltin --> End
    
    %% Additional information
    subgraph "Environment Handling"
        ExpandVars --> ConvertEnv[Convert t_env to char **envp]
        ConvertEnv --> FreeTempEnv[Free Temporary envp]
    end
    
    subgraph "Redirection Handling"
        SetupRedirs --> InRedir[Handle Input Redirections]
        SetupRedirs --> OutRedir[Handle Output Redirections]
        SetupRedirs --> HereDoc[Setup Heredoc Input]
        SetupRedirs --> Append[Handle Append Redirections]
    end
    
    subgraph "Path Resolution"
        FindPath --> CheckSlash{Contains '/'?}
        CheckSlash -- Yes --> UseAsPath[Use as Direct Path]
        CheckSlash -- No --> GetPATH[Get PATH Environment]
        GetPATH --> TryPaths[Try Each Directory in PATH]
        TryPaths --> CheckFound{Found?}
        CheckFound -- Yes --> ReturnPath[Return Full Path]
        CheckFound -- No --> ReturnNull[Return Original Command]
    end
    
    %% Memory Management
    subgraph "Memory Management"
        ConvertEnv --> CallCallocArray[ft_calloc for Arrays]
        FindPath --> StrdupPaths[strdup for Paths]
        CommandLoop --> FreeEnvp[safe_doube_star_free for envp]
        ChildExit --> FreeExecResources[Free Resources]
        End --> FinalCleanup[Cleanup Resources]
    end
```

## Key Components

1. **Command Execution Decision**
   - Parent builtins (cd, export, unset, exit) run in parent process
   - Other commands run in child processes

2. **Piping and Redirection**
   - Pipes connect stdout of one command to stdin of next command
   - Redirections modify stdin/stdout for each command

3. **Path Resolution**
   - Direct paths (containing '/') are used as-is
   - Other commands are searched in PATH directories

4. **Process Management**
   - Child processes execute commands and exit
   - Parent process waits for and collects exit status

5. **Environment Management**
   - Internal t_env list is converted to execve-compatible format
   - Proper memory management for environment variables

6. **Heredoc Handling**
   - All heredocs are set up before execution starts
   - Heredoc file descriptors are managed during execution