# Example Command Execution Flow (Step-by-Step)

This flowchart matches the step-by-step explanation for:

echo hello | grep hello > outputfile

```mermaid
flowchart TB
    A[User Input: echo hello]
    A --> B[Tokenize with tokenizer]
    B --> C[Parse with build_command_list]
    C --> D[Create cmd1 for echo]
    D --> E[Create cmd2 for grep with output redirection]
    E --> F[Link commands as pipeline]
    F --> G[execute_command_list]
    G --> H[setup_all_heredocs]
    H --> I[setup_command_pipe for echo]
    I --> J[expand_command_args for echo]
    J --> K[fork for echo]
    K --> L{Child or Parent}
    L -- Child --> M[setup_child_io for echo]
    M --> N[apply_redirections for echo]
    N --> O[handle_builtin_command for echo]
    O --> P[exit echo child]
    L -- Parent --> Q[parent_process for echo]
    Q --> R[setup_command_pipe for grep]
    R --> S[expand_command_args for grep]
    S --> T[fork for grep]
    T --> U{Child or Parent}
    U -- Child --> V[setup_child_io for grep]
    V --> W[apply_redirections for grep]
    W --> X[handle_output_redir for grep]
    X --> Y[handle_external_command for grep]
    Y --> Z[exit grep child]
    U -- Parent --> AA[parent_process for grep]
    AA --> AB[wait_for_children]
    AB --> AC[shell_loop ready for next command]
```

---

This flowchart follows the plain-language explanation, showing each major step and decision in the execution of the pipeline command.