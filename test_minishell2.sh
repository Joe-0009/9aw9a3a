#!/usr/bin/env bash
#
# Test harness for your minishell.
# If you pass the path to the executable it will use that,
# otherwise it will default to ./minishell if it exists.
#
# Usage:
#   chmod +x test_minishell.sh
#   ./test_minishell.sh [ /path/to/minishell_executable ]
#

# determine which binary to test
if [ -n "$1" ]; then
  MINISHELL="$1"
elif [ -x ./minishell ]; then
  MINISHELL="./minishell"
else
  echo "Usage: $0 /path/to/minishell_executable"
  exit 1
fi

TMPDIR=$(mktemp -d)
declare -a CMDS

# === CD tests ===
# CMDS+=("cd ../../../../../.. && pwd") # Uses &&
# CMDS+=("cd /home/user42/ && pwd") # Uses &&
# CMDS+=("cd \$HOME/Bureau && pwd") # Uses &&
# CMDS+=("unset HOME; cd") # Uses ;
# CMDS+=("export HOME= ; cd") # Uses ;
CMDS+=("cd too many arguments")
CMDS+=("cd ./path_not_found")
# CMDS+=("mkdir -p a/b; cd a/b; rm -r ../../a; cd .. && echo \$PWD \$OLDPWD") # Uses ; and &&
# CMDS+=("CDPATH=/; cd home/.. && pwd") # Uses ; and &&
# CMDS+=("CDPATH=/; cd home/dir && pwd") # Uses ; and &&
# CMDS+=("CDPATH=/; cd . && pwd") # Uses ; and &&
# CMDS+=("CDPATH=/; cd .. && pwd") # Uses ; and &&

# === ECHO tests ===
# CMDS+=("echo -n -n -nnnn -nnnnm; echo a") # Uses ;
# CMDS+=("echo -n -nnn hello -n; echo a") # Uses ;

# === ENV & EXPORT tests ===
# CMDS+=("env; export FOO=bar; env | grep '^FOO='") # Uses ;
# CMDS+=("unset FOO; env | grep '^FOO='") # Uses ;
# CMDS+=("export VAR=a; export \$VAR=test; echo \$VAR \$a") # Uses ;

# === EXIT STATUS tests ===
# CMDS+=("cd random_cmd; echo \$?") # Uses ;
# CMDS+=("./file_that_is_not_an_executable || true; echo \$?") # Uses || and ;
# CMDS+=("cat bla || true; echo \$?") # Uses || and ;
# CMDS+=("not_cmd || true; echo \$?") # Uses || and ;

# === PARSING & SUBSTITUTION samples ===
# CMDS+=("echo \"\\s\"; echo \"\\\\s\"") # Uses ;
CMDS+=("echo \"12\\\"\"")
CMDS+=("echo \\>")
CMDS+=("echo \$USER\$NOTSET\\\\\$USER\$USER\\\\\$USERtest\$USER")
CMDS+=("echo bonjour \\; ls") # Escaped ;, should be okay
CMDS+=("echo \"'abc'\"")
CMDS+=("echo '\"abc\"'")

# === PIPE tests ===
CMDS+=("echo -e \"one\ntwo\" | cat | cat | wc -l")

# === REDIRECTION tests ===
# CMDS+=("printf \"A\nB\nC\n\" > out1; cat < out1") # Uses ;
# CMDS+=("echo hello > out2 >> out2; cat out2") # Uses ;

# === SYNTHAX (syntax error) tests ===
CMDS+=("|")
CMDS+=("echo bonjour > > out")

printf "\nRunning %d tests through bash and minishell...\n\n" "${#CMDS[@]}"

for i in "${!CMDS[@]}"; do
  cmd="${CMDS[i]}"
  echo "===== Test $((i+1)): $cmd ====="
  # run in bash
  bash_out="$TMPDIR/bash_out"
  bash -c "$cmd" &> "$bash_out"
  # run in minishell
  mini_out="$TMPDIR/mini_out"
  printf "%s\nexit\n" "$cmd" | "$MINISHELL" &> "$mini_out"

  # show outputs
  echo "--- bash  output ---"
  sed 's/^/| /' "$bash_out"
  echo "--- minishell output ---"
  sed 's/^/| /' "$mini_out"

  # diff
  if diff -u --label bash --label minishell "$bash_out" "$mini_out"; then
    echo "Result: PASS"
  else
    echo "Result: FAIL"
  fi
  echo
done

# cleanup
rm -rf "$TMPDIR"