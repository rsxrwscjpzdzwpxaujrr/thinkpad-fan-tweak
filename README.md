ThinkPad fan tweak
==================

A simple script that switches the fan to disengaged mode at high CPU temperatures, which helps lower the temperature of the CPU.

This works because the fan in disengaged mode spins faster than in adaptive mode with maximal speed level.

Tested on Lenovo Thinkpad x220, but may work on others.

### Install:
```
$ git clone https://github.com/mirai65536/thinkpad-fan-tweak
$ cd thinkpad-fan-tweak
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

After installation you should reload a systemd configuration with this command, or restart:
```
$ sudo systemctl daemon-reload
```

Then you can run the script with this command:
```
$ sudo systemctl start thinkpad-fan-tweak
```

And stop with this:
```
$ sudo systemctl stop thinkpad-fan-tweak
```

To run script with system start:
```
$ sudo systemctl enable thinkpad-fan-tweak
```

You can configure the script by creating file /etc/thinkpad-fan-tweak, format is:
```
hightemp=80
interval=5
```

Where:  
`hightemp` is temperature, at which fan switches to disengaged mode.  
`interval` is interval of checking CPU temperature in seconds.

Default values are 80 and 5 respectively.
