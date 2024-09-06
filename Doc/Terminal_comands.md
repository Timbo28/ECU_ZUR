## Terminal comands
* Enable motor
```
moto [pos] [on/off]
[pos] = fl, fr, rl, rr
[on/off] = 1, 0
examples:
    -Disable front right motor:
    moto fr 0
    -Get motor enable status list
    moto
```
* Set max load of the individual motor
```
motoload [pos] [load]
[pos] = fl, fr, rl, rr
[load] = 1% - 100%
examples:
    -Set rear right motor to 50% of the max torque:
    motoload rr 50
    -Get motor load status list
    motoload
```
* Get motor temperature
```
mototmp
```
* Get inverter temperature
```
invtmp
```
* Get accumulator temperature
```
accutmp
```
* Get accumulatore charge
```
accucrg
```
* set max torque
```
maxtqr get
maxtqr set [torque]
[torque] = 0 - 2140 Mn
```
* Set Pedal filer parameter
```
peres get
peres set [alpha]
[alpha] = 1 - 100, 100 = no filter  1 = strong filer
```