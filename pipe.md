```mermaid
flowchart TD
    subgraph setup_command_pipe
        A["setup_command_pipe() called"] --> B{"Is there\na next command?"}
        B -->|Yes| C["Call setup_pipe(pipe_fd)"]
        C -->|Success| D["Return 1 (success)"]
        C -->|Failure| E["Close prev_pipe_read\nReturn 0 (failure)"]
        B -->|No| F["Set pipe_fd[0] = -1\nSet pipe_fd[1] = -1\nReturn 1 (success)"]
    end
    
    subgraph execute_command_list
        G["Loop through commands"] --> H["Call setup_command_pipe()"]
        H --> I["Fork process"]
        I -->|Child| J["child_process()"]
        I -->|Parent| K["parent_process()"]
        K --> L["Save new prev_pipe_read\nContinue to next command"]
    end
    
    subgraph pipe_setup
        M["setup_pipe(pipe_fd)"] --> N["Call pipe(pipe_fd)"]
        N -->|Success| O["pipe_fd[0] = read end\npipe_fd[1] = write end"]
        N -->|Failure| P["Return -1 (error)"]
    end
    
    subgraph data_flow ["Data Flow Through Pipes"]
        Q["Command 1\nOutput"] --> R["pipe_fd[1]\nWrite End"]
        R --> S["pipe_fd[0]\nRead End"]
        S --> T["Command 2\nInput"]
    end