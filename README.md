<div align="center">
<img src="hummingbird.png" width="450px">

<a href="https://travis-ci.org/Sweets/hummingbird"><img src="https://travis-ci.org/Sweets/hummingbird.svg?branch=master"></a>
<a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg"></a>
<img src="https://img.shields.io/badge/dependencies-None-navy.svg">
<img src="https://img.shields.io/badge/platform-Linux-lightgray.svg">
<img src="https://img.shields.io/github/repo-size/Sweets/hummingbird.svg?color=green">

<h6><i>lightning fast Linux init</i></h6>
</div>

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
