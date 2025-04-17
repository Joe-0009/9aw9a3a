```mermaid
flowchart TD
    A[pwd Command Invoked] --> B[Call getcwd]
    B --> C{getcwd Success}
    C -- Yes --> D[Print Working Directory]
    D --> E[Free Memory]
    E --> F[Return 0]
    C -- No --> G[Return 1]
```
