#!/bin/bash

# Alias-Name, der entfernt werden soll
alias_name="$1"

# Entferne den Alias aus der aktuellen Shell
unalias "$alias_name" 2>/dev/null

# Entferne den Alias aus ~/.bashrc oder ~/.bash_aliases
if grep -q "alias $alias_name=" ~/.bashrc; then
    sed -i "/alias $alias_name=/d" ~/.bashrc
    echo "Alias '$alias_name' removed from ~/.bashrc."
else
    echo -e "\e[31mAlias '$alias_name' not found in ~/.bashrc.\e[0m"
fi

# Neu laden der Konfigurationsdatei
source ~/.bashrc
