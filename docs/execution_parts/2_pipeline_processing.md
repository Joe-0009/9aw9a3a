# Pipeline Processing Flow

This diagram shows how minishell processes commands in a pipeline with detailed function parameters and behaviors.

```mermaid
flowchart TD
    Start([Pipeline Processing]) --> CommandLoop["Loop: for each command in cmd_list"]
    
    %% Command processing loop with details
    CommandLoop --> SetupPipe["setup_command_pipe(current, &prev_pipe_read, pipe_fd)"]
    SetupPipe --> ExpandVars["expand_command_args(current, env_list_to_envp(*env_list))"]
    ExpandVars --> ForkProcess["pid = fork()"]
    ForkProcess --> ProcessType{"pid value?"}
    
    %% Parent process flow with details
    ProcessType -- "pid > 0\n(Parent)" --> CloseHeredocs["close_heredoc_fds(current->redirections)"]
    CloseHeredocs --> ManagePipes["prev_pipe_read = parent_process(prev_pipe_read, pipe_fd)"]
    ManagePipes --> NextCommand{"current->next != NULL?"}
    NextCommand -- "Yes" --> IncrementCurrent["current = current->next"]
    IncrementCurrent --> CommandLoop
    NextCommand -- "No" --> WaitChildren["wait_for_children(): Collect all child statuses"]
    WaitChildren --> CollectStatus["Get final exit status\nfrom last command"]
    CollectStatus --> End([Return Status])
    
    %% Error handling for fork
    ForkProcess -- "pid == -1\n(Error)" --> HandleForkError["handle_fork_error(current, prev_pipe_read, pipe_fd)"]
    HandleForkError --> ReturnError["Return Error Code 1"]
    
    %% Child process flow
    ProcessType -- "pid == 0\n(Child)" --> ChildProcess([Go to Child Process Flow])
    
    %% Details about pipe setup
    subgraph "Pipe Setup Details"
        SetupPipe --> CheckNext{"current->next != NULL?"}
        CheckNext -- "Yes" --> CreatePipe["pipe(pipe_fd):\nCreate pipe for output"]
        CheckNext -- "No" --> NoPipe["pipe_fd set to -1:\nNo pipe needed"]
        
        CreatePipe -- "Success" --> PipeReady["Pipe ready for I/O"]
        CreatePipe -- "Failure" --> PipeError["Close prev_pipe_read\nReturn 0 (error)"]
    end
    
    %% Details about environment handling
    subgraph "Environment Handling for Expansion"
        ExpandVars --> CreateEnvp["env_list_to_envp(*env_list)"]
        CreateEnvp --> ExpandVariables["Expand $VARS in command arguments"]
        ExpandVariables --> ExpandRedirs["Expand $VARS in redirections"]
        ExpandRedirs --> FreeEnvp["safe_doube_star_free() handles cleanup internally"]
    end
```

## Key Functions Details

- `setup_command_pipe(t_command *current, int *prev_pipe_read, int pipe_fd[2])`:
  - Creates pipes between commands in a pipeline
  - For last command: sets pipe_fd to -1 (no output pipe needed)
  - For other commands: creates pipe using pipe() system call
  - Returns: 1 on success, 0 on failure (pipe creation error)

- `expand_command_args(t_command *cmd, char **envp)`:
  - Expands environment variables in command arguments and redirections
  - Handles expansion of $VARIABLES to their values
  - Takes temporary envp array that is freed internally
  - After expansion: command is ready for execution with real values

- `parent_process(int prev_pipe_read, int pipe_fd[2])`:
  - Manages parent's pipe file descriptors after fork
  - Closes pipe_fd[1] (write end) as parent doesn't write
  - Closes prev_pipe_read if it was open
  - Returns: pipe_fd[0] (read end) for next command or -1

- `close_heredoc_fds(t_redirections *redir)`:
  - Closes all heredoc file descriptors to prevent leaks
  - Only closes FDs for TOKEN_HEREDOC type redirections
  - Prevents resource leaks in parent process

- `wait_for_children()`:
  - Uses waitpid() to wait for all child processes
  - Collects exit status using WEXITSTATUS macro
  - Sets global exit status for "$?" expansion
  - Returns: Exit status of last command in pipeline