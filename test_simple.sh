#!/usr/bin/env bash
#
# Run a series of commands in one single minishell session
# so that built‑in state (cd, export, unset) carries over.

MINISHELL=./minishell

# Build a here‑doc containing all of your test commands
read -r -d '' TEST_CMDS << 'EOF'
pwd
cd /
pwd
cd nonexistent
echo cd failed (\$?)
echo hello world
export FOO=bar
echo \$FOO
unset FOO
echo FOO is '\$FOO'
not_a_cmd
exit
EOF

echo "Feeding the following commands to minishell:"
echo "-------------------------------------------"
echo "$TEST_CMDS"
echo "-------------------------------------------"
printf "%s\n" "$TEST_CMDS" | "$MINISHELL"