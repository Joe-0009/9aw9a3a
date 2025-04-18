#!/bin/bash

# Automated test script for minishell export, unset, and env builtins
./minishell <<EOF
export hello
export hello1=
export hello2="hi"
export | grep hello
env | grep hello
unset hello
unset hello1
export | grep hello
env | grep hello
export foo=bar
export foo="baz"
export | grep foo
unset foo
export | grep foo
exit
EOF
