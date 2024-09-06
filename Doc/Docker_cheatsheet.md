#Docker
```
See images:
ubuntu@Jetson:~$ sudo docker images
See running container:
ubuntu@Jetson:~$ sudo docker ps
Build image:
ubuntu@Jetson:~$ sudo docker build -t ecu_zur .
Run the container in detached mode:
ubuntu@Jetson:~$ sudo docker run --privileged --network=host -it --restart on-failure --name ecu_zur ecu_zur
Get to console of container(attached to container):
ubuntu@Jetson:~$ sudo docker attach 'container-id'
Exit attach mode:
ctrl+P and after ctrl+Q
Go into container to read files:
ubuntu@Jetson:~$ sudo docker exec -ti ecu_zur bash
Exit container file read mode:
root@container:~$ exit
Stop container:
ubuntu@Jetson:~$ sudo docker stop 'container-id'
Remove stoped container:
ubuntu@Jetson:~$ sudo docker container prune
alle images löschen:
ubuntu@Jetson:~$ sudo docker rmi $(sudo docker images -q)
```