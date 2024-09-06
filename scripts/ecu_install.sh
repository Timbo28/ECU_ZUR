#!/bin/bash


# Add the enable_jetson script to the crontab file
add_crontab() {
    # Überprüfen, ob der Befehl bereits in der Crontab-Datei vorhanden ist
    if sudo crontab -l | grep -q "$script_dir/$1"; then
        echo -e "\e[32mDer Befehl ist bereits in der Crontab-Datei vorhanden.\e[0m"
    else
        # Befehl zur Crontab-Datei hinzufügen
        (crontab -l >/dev/null; echo "@reboot $script_dir/$1") | sudo crontab -
        echo -e "\e[32mDer Befehl wurde erfolgreich zur Crontab-Datei hinzugefügt.\e[0m"
    fi
}

# Function to check where the ECU is installed
function ask_for_platform() {
    while true; do
        read -p "Would you like to install the ECU on the Nvidia Jetson or another PC? (jetson/pc): " answer

        if [ "$answer" == "jetson" ]; then
            echo "You have chosen Nvidia Jetson."
            
            #check if busybox is installed
            if ! command -v busybox &> /dev/null; then
                echo -e "\e[31mBusybox is not installed. Installing Busybox...\e[0m"
                sudo apt-get install busybox
            else
                echo -e "\e[32mBusybox is already installed.\e[0m"
            fi


            chmod +x $script_dir/enable_jetson_can.sh
            $script_dir/enable_jetson_can.sh
            add_crontab enable_jetson_can.sh

            break  # Exit the loop as the input is valid
        elif [ "$answer" == "pc" ]; then
            echo "You have chosen PC."
            ask_physical_can_buses
            break  # Exit the loop as the input is valid
        else
            echo "Invalid input. Please enter 'jetson' or 'pc'."
        fi
    done
}

# Function to ask if the PC has physical CAN buses
function ask_physical_can_buses() {
    while true; do
        read -p "Does the PC have physical CAN buses? (yes/no): " answer
        if [ "$answer" == "yes" ]; then
            echo "You have chosen PC with physical CAN buses."
            chmod +x $script_dir/enable_can.sh
            $script_dir/enable_can.sh
            add_crontab enable_can.sh
            break  # Exit the loop as the input is valid
        elif [ "$answer" == "no" ]; then
            echo "You have chosen PC without physical CAN buses."
            chmod +x $script_dir/enable_vcan.sh
            $script_dir/enable_vcan.sh $can_bus_count
            add_crontab "enable_vcan.sh $can_bus_count"
            break  # Exit the loop as the input is valid
        else
            echo "Invalid input. Please enter 'yes' or 'no'."
        fi
    done
}

# Function to install development environment
function install_dev_environment() {
    while true; do
        read -p "Would you like to install the development environment? (yes/no): " answer
        if [ "$answer" == "yes" ]; then
                
            
            # Check if gcc and g++ version is about 12
            gcc_version=$(gcc -dumpversion)
            gpp_version=$(g++ -dumpversion)
            if [ ${gcc_version:0:2} -ge 12 ] && [ ${gpp_version:0:2} -ge 12 ]; then
                echo -e "\e[32mgcc and g++ version is greater equal than 12.\e[0m"
            else
                echo -e "\e[31mgcc and g++ version is less than 12. updating gcc and g++...\e[0m"

                # Install gcc-12 and g++-12
                sudo apt install -y gcc-12 g++-12

                # Set gcc-12 and g++-12 as the default versions
                # Adding gcc-12 as an alternative
                sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 50
                # Adding g++-12 as an alternative
                sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 50

                # Automatically select the default version by setting priority
                # Update gcc alternatives
                sudo update-alternatives --set gcc /usr/bin/gcc-12

                # Update g++ alternatives
                sudo update-alternatives --set g++ /usr/bin/g++-12

            fi

            #installing cmake
            if ! command -v cmake &> /dev/null; then
                echo -e "\e[31mCMake is not installed. Installing CMake...\e[0m"
                sudo apt-get install cmake
            else
                echo -e "\e[32mCMake is already installed.\e[0m"
            fi

            break  # Exit the loop as the input is valid
        elif [ "$answer" == "no" ]; then
            break  # Exit the loop as the input is valid
        else
            echo "Invalid input. Please enter 'yes' or 'no'."
        fi
    done
}

# start of the script
sudo apt update
script_dir="$(dirname "$(realpath "$0")")"
ecu_dir="$(dirname "$script_dir")"



# check if jq is installed
if ! command -v jq &> /dev/null; then
    echo -e "\e[31mjq is not installed. Installing jq...\e[0m"
    sudo apt-get install jq
else
    echo -e "\e[32mjq is already installed.\e[0m"
fi

# save number of can buses
can_bus_count=$(jq 'length' $ecu_dir/.config/CanFrames.json)
echo -e "\e[34mNumber of CAN buses: $can_bus_count\e[0m"


# Check if Docker is already installed
if ! command -v docker &> /dev/null; then
    echo -e "\e[31mDocker is not installed. Installing Docker...\e[0m"

    sudo apt install docker.io
     
    if ! command -v docker &> /dev/null; then
        echo -e "\e[31m It is not possible to download Docker!\e[0m"
    else
        # make docker run without sudo
        if ! getent group docker > /dev/null; then
            echo "Docker-Gruppe wird erstellt..."
            sudo groupadd docker
        else
            echo "Docker-Gruppe existiert bereits."
        fi
        sudo usermod -aG docker $USER
        #newgrp docker 
    fi
else
    echo -e "\e[32mDocker is already installed.\e[0m"

fi






#check if can-utils is installed
if ! command -v cansend &> /dev/null; then
    echo -e "\e[31mcan-utils is not installed. Installing can-utils...\e[0m"
    sudo apt-get install can-utils
else
    echo -e "\e[32mcan-utils is already installed.\e[0m"
fi

ask_for_platform 
install_dev_environment

# load git submodules
git submodule update --init --recursive

chmod +x $script_dir/run.sh
chmod +x $script_dir/log.sh
chmod +x $script_dir/add_alias.sh
chmod +x $script_dir/remove_alias.sh

source $script_dir/add_alias.sh ecu_run $script_dir/run.sh
source $script_dir/add_alias.sh ecu_log $script_dir/log.sh

exit 0




