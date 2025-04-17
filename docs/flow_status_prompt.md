```mermaid
flowchart TD
    A[Command Execution Ends] --> B[Collect Exit Status]
    B --> C[Store in g_last_exit_status]
    C --> D[Display Prompt Again]
```
