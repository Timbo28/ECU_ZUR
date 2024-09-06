#!/bin/bash

# Define the alias
alias_command="alias $1='sudo $2'"

# Check if the alias already exists in ~/.bashrc or ~/.bash_aliases
if grep -Fxq "$alias_command" ~/.bashrc ; then
    echo -e "\e[31mAlias '$1' already exists.\e[0m"
else
    # Add the alias to ~/.bashrc
    echo "$alias_command" >> ~/.bashrc
    echo -e "\e[32mAlias '$1' added to ~/.bashrc. Close and open again your terminal to use the alias.\e[0m"

fi

# Reload the shell configuration to make the alias available immediately
source ~/.bashrc

