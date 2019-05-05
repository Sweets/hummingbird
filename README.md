# hummingbird
hummingbird is an init system designed to be fast, and to be specifically for Linux.

The most ideal setup for hummingbird is to have your system shell set to dash (this can be done by linking `/bin/sh` to dash's binary).

It is BSD-esque in configuration, meaning that to add start programs at boot-time you edit `/etc/rc.local`.

|Files|Purpose|
|-|-|
|`/etc/rc.init`|Mounts kernel API directories, loads system configuration (sysctl), turns swap on, sets hostname, sets up loopback device, starts TTYs|
|`/etc/rc.local`|Executes various user-defined operations (runs after `/etc/rc.init`)|
|`/etc/rc.shutdown`|Executes queued I/O write ops, turns off swap, unmounts mounted filesystems|
|`/etc/rc.shutdown.local`|Executes various user-defined operations (runs before `/etc/rc.shutdown`)|
