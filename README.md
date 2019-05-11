## hummingbird
hummingbird is an init system designed specifically for Linux and to yield fast boot times.

## Installation

### Building

```
$ git clone https://github.com/Sweets/hummingbird
$ cd hummingbird
$ make
$ sudo make install
$ sudo install -Dm755 ./etc/rc.init /etc/rc.init
$ sudo install -Dm755 ./etc/rc.shutdown /etc/rc.shutdown
```

Note: copying the rc files is separate from the Makefile to prevent users from overriding their pre-existing RCs. If you are switching from another init that uses `/etc/rc.shutdown` (or `/etc/rc.init` in the unlikely case), be sure to back these up if deemed necessary.

### Setting hummingbird as your init

To set hummingbird as your init you will need to add or change the `init` flag in your kernel boot parameters.

`init=/usr/bin/hummingbird`

Note for advanced users: if your goal in using hummingbird is to obtain the fastest achievable boot time for your hardware, consider stripping the Linux kernel of unnecessary drivers that don't pertain to your hardware.

## Configuration

hummingbird is BSD-esque in its configuration, meaning that to execute commands during boot (or the contrary, have them execute during shutdown) you add instructions to `.local` files.

By default no `.local` files exist.

|Path|Runtime|
|-|-|
|`/etc/rc.local`|Last during boot before spawning TTYs|
|`/etc/rc.shutdown.local`|First during shutdown|

Note: If you choose to create `.local` files, be sure that you set them to be executable (`chmod +x /etc/...`).

## Emergency Shell

If for any reason during the system's boot time the init fails, hummingbird immediately drops you into an emergency shell. This shell is whatever is linked to `/bin/sh`.

While in the emergency shell, you have full root access to the system. From here you may edit the rc files to fix and repair the init as needed.

After saving your changes to the rc files, you may attempt to initialize the system again or reboot.

If you choose to attempt to initialize the system again, simply run `sh /etc/rc.init` in the emergency shell. If it fails again, you will be dropped back into the shell.

If you choose to restart the system, you will need to run the `sync` command (this will commit any cached or queued I/O operations that the kernel has not done on the disk yet. Failure to do this command before restarting your machine from the emergency shell will cause any file changes to not be written to the disk).

If the emergency shell is exited (`Ctrl+D`, `$ exit`, etc) a kernel panic will be raised.

## Frequently Asked and Pre-empted Questions

### I can no longer use X after switching to hummingbird

Over the years, a lot of hacks have been applied to various bits of software to make X work "seemlessly". Systemd's logind automatically applies specific ownership to files in order to allow system-level files to be accessed by a non-root user. startx itself is also quite a bit of a hack, but too much so to explain here.

In order to start the X server again, you will need to add the following line to the file `/etc/X11/Xwrapper.config` (if the file does not exist -- which is likely -- create it)

```
needs_root_rights = yes
```

### I no longer have an internet connection (even through ethernet)

This is because the given device has not been set up and dhcp has not been ran. Most other init systems would do this automatically, but something to keep in mind is that hummingbird is _only_ an init; it does nothing more and nothing less than boot the system.

Get your specific device id that you wish to use from `ip link`.

Example of `ip link` output

```
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
  link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: enp0s3: <BROADCAST,MULTICAST> mtu 1500 qdisc fq_codel state DOWN mode DEFAULT group default qlen 1000
  link/ether 08:00:27:95:1d:c4 brd ff:ff:ff:ff:ff:ff
```

In this specific example, the ethernet device is labeled as `enp0s3`, which we will setup in our `/etc/rc.local` file.

```
#!/bin/sh

ip link set dev enp0s3 up
dhcpcd
```

Afterwards, you should have an internet connection (for wireless connections, you may need to use `wpa_supplicant`)

### Plymouth soft locks my system or is shown after the system is finished booting

hummingbird was tested and is known to be working with plymouth, but in testing, plymouth was slower to start than hummingbird could initialize the system. By the time the plymouth screen loaded up, a user was already able to login to a terminal, causing the screen to be taken by plymouth and appear to be "soft locked".

To fix this, you need to make sure that plymouth is told to quit when hummingbird has initialized the system. In the event that you want to see the plymouth screen regardless of boot time, you may artificially delay the boot process by putting `sleep` in the `.local` file.

```
#!/bin/sh

sleep 5
plymouth --quit
```

In addition to the above, you may want to lower the `ShowDelay` variable in `/etc/plymouth/plymouth.conf` to see plymouth earlier.

### My question isn't listed!

Open an issue! I'll try to get back to you as soon as possible.
