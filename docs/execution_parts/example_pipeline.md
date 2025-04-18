# Example Command Execution Flow

This file explains in detail how the command `echo "hello" | grep hello > outputfile` is executed in your minishell implementation.

```mermaid
flowchart TB
  %% Vertical flow for pipeline with redirection
  Start[Input: echo hello | grep hello > outputfile]
  Start --> Tokenize[tokenizer(input_line)]
  Tokenize --> Parse[build_command_list(token_list)]
  Parse --> ExecList[execute_command_list(cmd_list, env_list)]
  
  %% Heredoc setup (none here)
  ExecList --> SetupHeredocs[setup_all_heredocs(cmd_list)]
  
  %% First command (echo)
  SetupHeredocs --> SetupPipe1[setup_command_pipe(cmd1, &prev_pipe_read, pipe_fd)]
  SetupPipe1 --> Expand1[expand_command_args(cmd1, env_list_to_envp(env_list))]
  Expand1 --> Fork1[pid1 = fork()]
  
  Fork1 -->|Child| Child1[child_process(cmd1, prev_pipe_read, pipe_fd, env_list)]
  Child1 --> Io1[setup_child_io(prev_pipe_read, pipe_fd)]
  Io1 --> Redir1[apply_redirections(cmd1)]
  Redir1 --> Exec1[execute_single_command(cmd1, env_list)]
  Exec1 --> Exit1[handle_builtin_command() -> builtin_echo() -> exit(status)]
  
  Fork1 -->|Parent| Parent1[parent_process(prev_pipe_read, pipe_fd)]
  Parent1 --> CloseHeredocs1[close_heredoc_fds(redirs)]
  Parent1 --> NextCmd1[current = cmd1->next]
  
  %% Second command (grep)
  NextCmd1 --> SetupPipe2[setup_command_pipe(cmd2, &prev_pipe_read, pipe_fd)]
  SetupPipe2 --> Expand2[expand_command_args(cmd2, env_list_to_envp(env_list))]
  Expand2 --> Fork2[pid2 = fork()]
  
  Fork2 -->|Child| Child2[child_process(cmd2, prev_pipe_read, pipe_fd, env_list)]
  Child2 --> Io2[setup_child_io(prev_pipe_read, pipe_fd)]
  Io2 --> Redir2[handle_output_redir(redir2) -> open("outputfile"), dup2(file, STDOUT)]
  Redir2 --> Exec2[handle_external_command() -> get_exec_path() -> execve()]
  Exec2 --> Exit2[exit(status)]
  
  Fork2 -->|Parent| Parent2[wait_for_children() -> waitpid()]
  Parent2 --> End[Return to shell_loop()]

  %% Data flow subgraph
  subgraph "Data Flow"
    direction TB
    D1["echo writes 'hello' to pipe"]
    D1 --> D2["grep reads from pipe, writes to file"]
  end
```

## Step-by-Step Breakdown

1. **Parsing & Command Construction**
   - `tokenizer(input_line)` splits the input into tokens.
   - `build_command_list()` creates two `t_command` structs:
     - `cmd1` for `echo hello`
     - `cmd2` for `grep hello` with output redirection to `outputfile`

2. **Execution Start**
   - `execute_command_list()` detects a pipeline and calls `setup_all_heredocs()` (no heredocs here).
   - Initializes `pipe_fd` and `prev_pipe_read`.

3. **Echo Command Execution**
   - `setup_command_pipe(cmd1, &...)` creates a new pipe.
   - `expand_command_args(cmd1, envp)` expands any variables and frees `envp`.
   - `fork()` creates a child process:
     - **Child**: Calls `child_process()`, then `setup_child_io()` redirects `STDOUT` to the pipe.
       - `apply_redirections()` does nothing for echo.
       - `execute_single_command()` runs `handle_builtin_command()` → `builtin_echo()`, writing "hello" to the pipe.
       - Child exits with status 0.
     - **Parent**: Calls `parent_process()` to close write end, save read end in `prev_pipe_read`, and continues.

4. **Grep Command Execution**
   - `setup_command_pipe(cmd2, &...)` sees no next command, so skips pipe creation.
   - `expand_command_args(cmd2, envp)` expands variables and frees `envp`.
   - `fork()` creates a second child:
     - **Child**: Calls `child_process()`, then `setup_child_io()` redirects `STDIN` from the pipe.
       - `handle_output_redir()` opens `outputfile` and redirects `STDOUT` to the file.
       - `handle_external_command()` calls `get_exec_path()` and `execve()` to run `/usr/bin/grep`.
       - Grep reads "hello" from `STDIN`, finds a match, writes "hello" to `outputfile`.
       - Child exits with grep's status.
     - **Parent**: Calls `wait_for_children()` to reap both children and returns the last exit status.

5. **Completion**
   - `shell_loop()` resumes, ready for the next command.

The file `outputfile` now contains the line:
```
hello
```