#!/bin/bash

# Function to execute and check commands
run_command() {
    $1
    if [ $? -ne 0 ]; then
        echo "Error: The command '$1' has failed."
        exit 1
    fi
}

# Check if exactly one argument is passed
if [ $# -ne 1 ]; then
  echo "Error: Exactly one argument must be provided."
  exit 1
fi

# Check if the argument is a number between 0 and 10
if ! [[ "$1" =~ ^[0-9]$|^10$ ]]; then
    echo "Error: The argument must be a number between 0 and 10."
    exit 1
fi

# Find all CAN interfaces
CAN_INTERFACES=$(ip -o link show | awk -F': ' '/can[0-9]+/{print $2}')

# Shut down and remove each CAN interface
for iface in $CAN_INTERFACES; do
    run_command "sudo ip link set $iface down"
    run_command "sudo ip link delete $iface type can"
done

# Load the vcan module
run_command "sudo modprobe vcan"

# Add and bring up the vCAN interfaces
num_can_interfaces=$1 
echo "Creating $num_can_interfaces vCAN interfaces..."
for ((i = 0; i < num_can_interfaces; i++)); do
    run_command "sudo ip link add dev can$i type vcan"
    run_command "sudo ip link set up can$i"
done

exit 0
