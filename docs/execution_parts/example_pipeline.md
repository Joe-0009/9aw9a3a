# Example Command Execution Flow: `echo "hello" | grep hello > outputfile`

This diagram shows the detailed execution process for the specific command `echo "hello" | grep hello > outputfile` with a vertical layout emphasizing the sequence of operations.

```mermaid
flowchart TB
    %% Starting point
    Start([User Input: echo "hello" | grep hello > outputfile]) --> Tokenize
    
    %% Parsing phase (vertical)
    Tokenize["tokenizer(input_line)
    - TOKEN_WORD: echo
    - TOKEN_WORD: \"hello\"
    - TOKEN_PIPE: |
    - TOKEN_WORD: grep
    - TOKEN_WORD: hello
    - TOKEN_REDIRECT_OUT: >
    - TOKEN_WORD: outputfile"] --> BuildCmds
    
    BuildCmds["build_command_list()
    - cmd1: echo \"hello\"
    - cmd2: grep hello > outputfile
    - cmd1->next = cmd2"] --> Process
    
    Process["process_command()
    - Calls execute_command_list()"] --> ExecuteCmds
    
    ExecuteCmds["execute_command_list()
    - Detects pipeline (cmd->next != NULL)
    - Not a parent builtin"] --> SetupDocs
    
    SetupDocs["setup_all_heredocs()
    - No heredocs in this command"] --> InitVars
    
    InitVars["Initialize variables
    - pipe_fd[2] = {-1, -1}
    - prev_pipe_read = -1"] --> EchoCmd
    
    %% First command: echo (vertical)
    EchoCmd["current = cmd_list (echo command)"] --> SetupEchoPipe
    
    SetupEchoPipe["setup_command_pipe()
    - pipe(pipe_fd) creates pipe
    - pipe_fd[0] = read end
    - pipe_fd[1] = write end"] --> ExpandEchoVars
    
    ExpandEchoVars["expand_command_args()
    - env_list_to_envp() for environment
    - Expands \"hello\" (no variables)
    - Frees temporary envp"] --> ForkEcho
    
    ForkEcho["pid = fork()"] --> ForkEchoDecision
    
    ForkEchoDecision{{"Fork result?"}} --> |"Child process (pid=0)"| EchoChild
    ForkEchoDecision --> |"Parent process (pid>0)"| EchoParent
    
    %% Echo child process branch (vertical)
    EchoChild["child_process() for echo"] --> SetupEchoChildIO
    
    SetupEchoChildIO["setup_child_io()
    - stdin remains unchanged
    - stdout redirected to pipe_fd[1]
    - dup2(pipe_fd[1], STDOUT_FILENO)
    - close(pipe_fd[0])"] --> ApplyEchoRedirs
    
    ApplyEchoRedirs["apply_redirections()
    - No redirections for echo"] --> ExecuteEcho
    
    ExecuteEcho["execute_single_command()"] --> EchoIsBuiltin
    
    EchoIsBuiltin{{"is_builtin_command(\"echo\")?"}} --> |"Yes"| RunEchoBuiltin
    
    RunEchoBuiltin["handle_builtin_command()
    - builtin_echo() executes
    - Writes \"hello\" to stdout (pipe)"] --> ExitEcho
    
    ExitEcho["exit(builtin_status)
    - Child process terminates"] --> %%%
    
    %% Echo parent process branch (going right then down)
    EchoParent["Parent continues execution"] --> CloseEchoHeredocs
    
    CloseEchoHeredocs["close_heredoc_fds()
    - No heredocs to close"] --> ManageEchoPipe
    
    ManageEchoPipe["parent_process()
    - close(pipe_fd[1]) (write end)
    - prev_pipe_read = pipe_fd[0]
    - Save read end for next command"] --> NextCmd
    
    %% Second command: grep (vertical)
    NextCmd["current = current->next (grep command)"] --> SetupGrepPipe
    
    SetupGrepPipe["setup_command_pipe()
    - No pipe needed (last command)
    - pipe_fd set to {-1, -1}"] --> ExpandGrepVars
    
    ExpandGrepVars["expand_command_args()
    - env_list_to_envp() for environment
    - Expands args (no variables)
    - Frees temporary envp"] --> ForkGrep
    
    ForkGrep["pid = fork()"] --> ForkGrepDecision
    
    ForkGrepDecision{{"Fork result?"}} --> |"Child process (pid=0)"| GrepChild
    ForkGrepDecision --> |"Parent process (pid>0)"| GrepParent
    
    %% Grep child process branch (vertical)
    GrepChild["child_process() for grep"] --> SetupGrepChildIO
    
    SetupGrepChildIO["setup_child_io()
    - stdin from prev_pipe_read
    - dup2(prev_pipe_read, STDIN_FILENO)
    - close unused FDs"] --> ApplyGrepRedirs
    
    ApplyGrepRedirs["apply_redirections()
    - handle_output_redir()
    - Opens outputfile (O_CREAT|O_TRUNC)
    - dup2(fd, STDOUT_FILENO)
    - stdout now goes to file"] --> ExecuteGrep
    
    ExecuteGrep["execute_single_command()"] --> GrepIsBuiltin
    
    GrepIsBuiltin{{"is_builtin_command(\"grep\")?"}} --> |"No"| HandleExternal
    
    HandleExternal["handle_external_command()"] --> GetGrepPath
    
    GetGrepPath["get_exec_path()
    - find_executable_path()
    - Searches PATH for grep binary"] --> ExecveGrep
    
    ExecveGrep["execve(path, args, envp)
    - Current process replaced by grep
    - Reads \"hello\" from stdin (pipe)
    - Matches pattern \"hello\"
    - Writes match to stdout (file)"] --> %%%
    
    %% Grep parent process branch (going right then down)
    GrepParent["Parent continues execution"] --> CloseGrepHeredocs
    
    CloseGrepHeredocs["close_heredoc_fds()
    - No heredocs to close"] --> ManageGrepPipe
    
    ManageGrepPipe["parent_process()
    - close(prev_pipe_read)
    - No more pipes needed"] --> LastCmdCheck
    
    LastCmdCheck{{"current->next == NULL?"}} --> |"Yes"| CloseAllPipes
    
    CloseAllPipes["safe_close(&prev_pipe_read)
    - Close any remaining FDs"] --> WaitForChildren
    
    WaitForChildren["wait_for_children()
    - waitpid(-1, &status, 0) in loop
    - Waits for both child processes
    - Collects exit status"] --> SetExitCode
    
    SetExitCode["Set $? to exit status
    - Last command's status (grep)"] --> ReturnToShellLoop
    
    ReturnToShellLoop["Return to shell_loop()
    - Ready for next command"] --> End([Command Execution Complete])
    
    %% File descriptors subgraph showing data flow
    subgraph "File Descriptor Flow"
        direction TB
        FD1["STDOUT (fd 1) in echo process"]
        FD2["pipe_fd[1] (write end)"]
        FD3["pipe_fd[0] (read end)"]
        FD4["STDIN (fd 0) in grep process"]
        FD5["STDOUT (fd 1) in grep process"]
        FD6["outputfile (file descriptor)"]
        
        FD1 -->|"dup2()"| FD2
        FD2 -->|"pipe data"| FD3
        FD3 -->|"dup2()"| FD4
        FD5 -->|"dup2()"| FD6
    end
    
    %% Data flow explanation
    subgraph "Data Flow Details"
        direction TB
        Data1["echo builtin generates \"hello\" text"]
        Data2["Data written to pipe through pipe_fd[1]"]
        Data3["grep reads \"hello\" from pipe through STDIN"]
        Data4["grep matches pattern \"hello\""]
        Data5["Match written to outputfile"]
        
        Data1 --> Data2 --> Data3 --> Data4 --> Data5
    end
```

## Detailed Function Call Sequence

1. **Input Processing & Command Structure Creation**
   - `tokenizer(input_line)`: Creates token list with appropriate token types
     - Token list: ["echo", "hello", PIPE, "grep", "hello", REDIRECT_OUT, "outputfile"]
   - `build_command_list(token_list)`: Builds command structures
     - Creates first command: `echo "hello"`
     - Creates second command: `grep hello` with OUTPUT redirection to "outputfile"
     - Links commands in a pipeline (cmd1->next = cmd2)

2. **Execution Initialization**
   - `process_command(cmd_list, env_list)`: Entry point for command execution
   - `execute_command_list(cmd_list, env_list)`: Main execution function 
     - Detects this is a pipeline (not a single parent builtin)
   - `setup_all_heredocs(cmd_list)`: None present in this command
   - Initialize pipe variables: pipe_fd[2] = {-1, -1}, prev_pipe_read = -1

3. **First Command (Echo) Processing**
   - `setup_command_pipe(current, &prev_pipe_read, pipe_fd)`: 
     - `pipe(pipe_fd)` creates kernel pipe with read/write ends
     - pipe_fd[0] = read end, pipe_fd[1] = write end
   - `expand_command_args(echo_cmd, env_list_to_envp(*env_list))`: 
     - Expands environment variables (none in "hello")
     - Frees temporary environment array
   - `pid1 = fork()`: Creates child process

4. **Echo Child Process**
   - `child_process(echo_cmd, prev_pipe_read, pipe_fd, *env_list)`: Sets up execution
   - `setup_child_io(prev_pipe_read, pipe_fd)`: 
     - `dup2(pipe_fd[1], STDOUT_FILENO)`: Redirects stdout to pipe write end
     - `close(pipe_fd[0])`: Closes read end (unused in this process)
   - `apply_redirections(current)`: No redirections for echo
   - `execute_single_command(current, env_list)`: Executes the command
   - `is_builtin_command("echo")`: Returns true
   - `handle_builtin_command(current, env_list)`: Handles builtin execution
     - `builtin_echo(cmd)`: Implements echo, writing "hello" to stdout (pipe)
   - `exit(builtin_status)`: Terminates child process with echo's status code

5. **Echo Parent Process**
   - `close_heredoc_fds(current->redirections)`: No heredocs to close
   - `prev_pipe_read = parent_process(prev_pipe_read, pipe_fd)`: 
     - `close(pipe_fd[1])`: Closes write end (unused in parent)
     - Saves pipe_fd[0] (read end) in prev_pipe_read for next command

6. **Second Command (Grep) Processing**
   - `current = current->next`: Move to grep command
   - `setup_command_pipe(current, &prev_pipe_read, pipe_fd)`: 
     - No pipe needed (last command)
     - Sets pipe_fd to {-1, -1}
   - `expand_command_args(grep_cmd, env_list_to_envp(*env_list))`: 
     - Expands variables (none present)
     - Frees temporary environment
   - `pid2 = fork()`: Creates second child process

7. **Grep Child Process**
   - `child_process(grep_cmd, prev_pipe_read, pipe_fd, *env_list)`: Sets up execution
   - `setup_child_io(prev_pipe_read, pipe_fd)`: 
     - `dup2(prev_pipe_read, STDIN_FILENO)`: Reads input from pipe
     - Closes unused file descriptors
   - `apply_redirections(current)`: Processes redirections
     - `handle_output_redir(redirection)`: 
       - `fd = open("outputfile", O_WRONLY|O_CREAT|O_TRUNC, 0644)`
       - `dup2(fd, STDOUT_FILENO)`: Redirects stdout to file
       - `close(fd)`: Closes original file descriptor
   - `execute_single_command(current, env_list)`: Executes grep
   - `is_builtin_command("grep")`: Returns false
   - `handle_external_command(current, env_list)`: Handles external command
     - `get_exec_path(current, env_list)`: Gets full path to grep executable
       - `find_executable_path(cmd, envp)`: Searches PATH
     - `envp = env_list_to_envp(env_list)`: Creates environment for execve
     - `execve(exec_path, current->args, envp)`: Replaces process with grep
       - Grep reads "hello" from stdin (pipe)
       - Matches it against pattern "hello" 
       - Writes match to stdout (redirected to outputfile)

8. **Grep Parent Process**
   - `close_heredoc_fds(current->redirections)`: No heredocs to close
   - `prev_pipe_read = parent_process(prev_pipe_read, pipe_fd)`: 
     - `close(prev_pipe_read)`: Closes pipe read end
   - `current->next == NULL`: True, pipeline ends
   - `safe_close(&prev_pipe_read)`: Ensures all pipe FDs are closed

9. **Completion & Status Collection**
   - `wait_for_children()`: Waits for all child processes
     - Uses `waitpid(-1, &status, 0)` in a loop
     - Collects exit status from both echo and grep
   - Sets global exit status ($?) from last command (grep)
   - Returns to shell_loop() for next command

10. **Result**
    - The string "hello" is written to outputfile through the pipeline