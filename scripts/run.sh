#!/bin/bash


cd "$(dirname "$0")/.."


sudo docker stop ecu_zur
sudo docker rm ecu_zur
sudo docker build --target build -t ecu_build .
sudo docker build -t ecu_zur .
sudo docker run --network=host -it  --restart on-failure --name ecu_zur ecu_zur
