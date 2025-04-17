```mermaid
flowchart TD
    A[env Command Invoked] --> B[Set i to 0]
    B --> C{More}
    C -- Yes --> D[Print envp i]
    D --> E[Increment i]
    E --> C
    C -- No --> F[Return 0]
```
