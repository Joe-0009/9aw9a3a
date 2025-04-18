# Path Resolution & Environment Flow

This diagram shows how minishell resolves executable paths and handles environment variables with additional implementation details.

```mermaid
flowchart TD
    Start([External Command]) --> GetPath["get_exec_path(current, env_list)"]
    
    %% Path resolution flow
    GetPath --> EnvConvert["envp = env_list_to_envp(env_list)"]
    EnvConvert --> FindPath["exec_path = find_executable_path(cmd, envp)"]
    FindPath --> FreeTemp["safe_doube_star_free(envp)"]
    FreeTemp --> CheckNull{"exec_path == NULL?"}
    CheckNull -- "Yes" --> DupCommand["return ft_strdup(cmd)"]
    CheckNull -- "No" --> ReturnPath["return exec_path"]
    
    %% Path finding logic
    FindPath --> CheckSlash{"Contains '/' ?"}
    CheckSlash -- "Yes" --> DirectPath["Try direct path with access()"]
    DirectPath -- "Accessible" --> UseAsPath["Return duplicate path"]
    DirectPath -- "Not accessible" --> ReturnNull1["Return NULL"]
    
    CheckSlash -- "No" --> GetPATH["path_value = get_path_value(envp)"]
    GetPATH --> CheckPATH{"PATH found?"}
    CheckPATH -- "No" --> ReturnNull2["Return NULL"]
    CheckPATH -- "Yes" --> SplitPATH["path_dirs = ft_split(path_value, ':')"]
    
    %% Try each directory
    SplitPATH --> TryDirs["For each directory in path_dirs"]
    TryDirs --> JoinPath["full_path = join_path(dir, cmd)"]
    JoinPath --> CheckAccess{"access(full_path, X_OK) == 0?"}
    CheckAccess -- "Yes" --> FoundPath["Clean up and return full_path"]
    CheckAccess -- "No" --> NextDir["Try next directory"]
    NextDir -- "More dirs" --> TryDirs
    NextDir -- "No more dirs" --> NotFound["Clean up and return NULL"]
    
    %% Environment conversion
    subgraph "Environment Conversion Details"
        EnvConvert --> CountEnv["Count variables in env_list"]
        CountEnv --> AllocEnv["envp = ft_calloc(count + 1, sizeof(char*))"]
        AllocEnv --> BuildEnv["For each env variable:"]
        BuildEnv --> FormatVar["Format as KEY=VALUE"]
        FormatVar --> StoreVar["Store in envp array"]
        StoreVar --> NullTerm["Set final entry to NULL"]
    end
```

## Key Functions Details

- `get_exec_path(t_command *current, t_env *env_list)`:
  - Creates temporary environment array using env_list_to_envp()
  - Calls find_executable_path() to locate the command
  - Frees temporary environment immediately after use
  - If command not found in PATH, returns a copy of the command name
  - Implementation detail: Uses ft_strdup() for returning paths

- `find_executable_path(char *cmd, char **envp)`:
  - First checks if cmd contains '/' - if so, validates as direct path
  - Otherwise gets PATH from environment and splits into directories
  - For each directory: Creates full path and checks using access()
  - Memory management: Properly frees split path array on all return paths
  - Return: Full resolved path that can be executed or NULL if not found

- `env_list_to_envp(t_env *env_list)`:
  - Now uses ft_calloc() instead of malloc() for safety (array zero-initialized)
  - Creates "KEY=VALUE" strings for each environment variable
  - For variables without values: Creates "KEY=" format
  - Implementation: Uses ft_strlcpy/ft_strlcat for safe string operations
  - Memory management: Properly handles allocation failures in sub-steps

- `join_path(char *dir, char *cmd)`:
  - Safely combines directory path and command name
  - Ensures proper '/' separator between directory and command
  - Implementation: Handles cases where directory already ends with '/'
  - Memory management: Returns newly allocated string that caller must free