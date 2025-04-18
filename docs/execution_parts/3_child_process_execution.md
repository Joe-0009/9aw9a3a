# Child Process Execution Flow

This diagram shows in detail how commands are executed in child processes including redirection handling and error management.

```mermaid
flowchart TD
    Start([Child Process]) --> SetupFds["child_process(current, prev_pipe_read, pipe_fd, env_list)"]
    
    %% Initial setup of FDs and redirections
    SetupFds --> SetupIo["setup_child_io(prev_pipe_read, pipe_fd)"]
    SetupIo --> ApplyRedirs["apply_redirections(current)"]
    ApplyRedirs --> CommandCheck{"execute_single_command(current, env_list)"}
    
    %% Command type branching with details
    CommandCheck -- "is_builtin_command(current->args[0])" --> ExecBuiltin["handle_builtin_command(current, env_list)"]
    CommandCheck -- "external command" --> FindPath["handle_external_command(current, env_list)"]
    
    %% External command path with details
    FindPath --> GetPath["exec_path = get_exec_path(current, env_list)"]
    GetPath --> CheckPath{"exec_path != NULL?"}
    CheckPath -- "Yes" --> PrepExec["envp = env_list_to_envp(env_list)"]
    PrepExec --> Execve["execve(exec_path, current->args, envp)"]
    CheckPath -- "No" --> CommandNotFound["Print command not found error"]
    
    %% Exit paths with detailed exit codes
    CommandNotFound --> ChildExit127["exit(127): Command not found"]
    Execve -- "execve returns (failure)" --> ExecError["perror('execve error')"]
    ExecError --> FreeResources["safe_free((void**)&exec_path)\nsafe_doube_star_free(envp)"]
    FreeResources --> ChildExit127
    ExecBuiltin --> ExitStatus["exit(builtin_status): Return builtin result"]
    
    %% Detailed redirection handling
    subgraph "Redirection Handling"
        ApplyRedirs --> ProcessRedirs["Process each t_redirections node"]
        ProcessRedirs --> RedirType{"Redirection Type?"}
        
        RedirType -- "TOKEN_INPUT (<)" --> HandleInput["handle_input_redir()\nOpen file for reading\nRedirect to STDIN"]
        RedirType -- "TOKEN_OUTPUT (>)" --> HandleOutput["handle_output_redir()\nCreate/truncate file\nRedirect to STDOUT"]
        RedirType -- "TOKEN_APPEND (>>)" --> HandleAppend["handle_append_redir()\nCreate/open file for append\nRedirect to STDOUT"]
        RedirType -- "TOKEN_HEREDOC (<<)" --> HandleHeredoc["handle_heredoc_redir()\nUse pre-created FD\nRedirect to STDIN"]
        
        HandleInput -- "Error opening" --> RedirError["Return -1 (error)"]
        HandleOutput -- "Error creating" --> RedirError
        HandleAppend -- "Error opening" --> RedirError
        HandleHeredoc -- "Invalid FD" --> RedirError
        
        RedirError --> ApplyExit["Command won't execute due to error"]
    end
    
    %% Pipe setup detail
    subgraph "File Descriptor Setup"
        SetupIo --> CheckPrevPipe{"prev_pipe_read >= 0?"}
        CheckPrevPipe -- "Yes" --> SetupInput["dup2(prev_pipe_read, STDIN_FILENO)\nClose prev_pipe_read"]
        
        SetupIo --> CheckNextPipe{"pipe_fd[1] >= 0?"}
        CheckNextPipe -- "Yes" --> SetupOutput["dup2(pipe_fd[1], STDOUT_FILENO)\nClose pipe_fd[1]"]
        
        SetupIo --> ClosePipe0["safe_close(&pipe_fd[0])"]
    end
```

## Key Functions Details

- `child_process(t_command *current, int prev_pipe_read, int pipe_fd[2], t_env *env_list)`:
  - Entry point for executing commands in child processes
  - Sets up input/output pipes for command in pipeline
  - Applies all redirections specified in command
  - Calls execute_single_command() to run the command
  - Never returns - process is replaced or exits with status code

- `setup_child_io(int prev_pipe_read, int pipe_fd[2])`:
  - Configures standard input/output for pipeline commands
  - Redirects stdin from prev_pipe_read if available
  - Redirects stdout to pipe_fd[1] if available
  - Closes unused pipe ends to prevent leaks
  - Returns: void - child process always continues

- `apply_redirections(t_command *cmd)`:
  - Processes all redirections in command (>, <, >>, <<)
  - Opens files and sets up file descriptors
  - For each redirection: calls appropriate handler function
  - Returns: 0 on success, -1 if any redirection fails

- `handle_external_command(t_command *current, t_env *env_list)`:
  - Resolves executable path using PATH environment
  - Converts environment to format needed by execve
  - Attempts to run command using execve() system call
  - On failure: frees resources and exits with status 127
  - Never returns on success (process is replaced by command)

- `handle_builtin_command(t_command *current, t_env *env_list)`:
  - Executes shell builtins in child process context
  - Captures return status from builtin function
  - Exits child process with builtin's status code
  - Handles: echo, pwd, env (non-parent builtins)