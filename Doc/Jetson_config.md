## Configuration
* Set Power mode to max
```
nvpmodel -m 3
```
* Enable Realtime
```
sudo sysctl -w kernel.sched_rt_runtime_us=-1
```

* Enable realtime permanently
Add
```
kernel.sched_rt_runtime_us=-1
```
to this file:

```
/etc/sysctl.conf
```
* auto start script after reboot
Add
```
@reboot /path/to/your/script.sh
```
To

```
sudo crontab -e

```
* add an alias to the pc
```
 nano ~/.bashrc
 ```
 add to bashrc
 ```
 alias run='sudo /path/to/your/run.sh'
```
run to load
```
source ~/.bashrc
```

* make script executable
```
chmod +x run.sh
```
