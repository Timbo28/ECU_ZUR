#!/bin/bash
script_dir="$(dirname "$(realpath "$0")")"
ecu_dir="$(dirname "$script_dir")"

#Check if jq is installed and deinstall jq
if [ -x "$(command -v jq)" ]; then
  echo "jq is installed. Deinstalling jq..."
  sudo apt-get remove jq -y
  echo "jq is deinstalled."
else
  echo "jq is not installed."
fi

#Check if can-utils is installed and deinstall can-utils
if [ -x "$(command -v cansend)" ]; then
  echo "can-utils is installed. Deinstalling can-utils..."
  sudo apt-get remove can-utils -y
  echo "can-utils is deinstalled."
else
  echo "can-utils is not installed."
fi

# remove aliases
source $script_dir/remove_alias.sh ecu_run
source $script_dir/remove_alias.sh ecu_log

#cromtab remove entrance wirh ECU 
sudo crontab -l | grep -v "/ECU/scripts/" |sudo crontab -
echo "Crontab entry for ECU removed."

# stop docker container and delete images
sudo docker stop ecu_zur
sudo docker rm ecu_zur
sudo docker rmi ecu_zur
sudo docker rm ecu_build
sudo docker rmi ecu_build
sudo docker container prune -f

# delite ECU directory
sudo rm -rf $ecu_dir

