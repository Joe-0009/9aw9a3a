#!/bin/bash

# Automated test script for minishell: pipes, redirections, and builtins
./minishell <<EOF
echo hello | cat
echo foo | grep foo | wc -l
echo bar > testfile && cat < testfile
echo one > f1 && echo two > f2 && cat f1 f2 | sort
echo "abc" | grep a | wc -c
echo "exported" | tee temp_exported | cat
echo "test" | cat | cat | cat
echo "hello world" | tr ' ' '\n' | sort | uniq
echo "42" | cat | grep 2
echo "done"
exit
EOF
