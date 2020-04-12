<div align="center">
<img src="hummingbird.png" width="350px">
<br><br>
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
$ sudo make seed_install
$ sudo install -Dm755 ./etc/rc.init /etc/rc.init
$ sudo install -Dm755 ./etc/rc.shutdown /etc/rc.shutdown
```

Note: copying the rc files is separate from the Makefile to prevent users from overriding their pre-existing RCs. If you are switching from another init that uses `/etc/rc.shutdown` (or `/etc/rc.init` in the unlikely case), be sure to back these up if deemed necessary.

### Setting hummingbird as your init

To set hummingbird as your init, you have two options.

Firstly, if you don't want to override whatever binary exists at `/bin/init`, you can simply append the `init` flag to your kernel parameters with the path to hummingbird.

`init=/usr/bin/hummingbird`

If you don't care or don't have an init installed, once `make install` has been ran, move the binary to `/bin/init`

`mv /usr/bin/hummingbird /bin/init`

Note for advanced users: if your goal in using hummingbird is to obtain the fastest achievable boot time for your hardware, consider stripping the Linux kernel of unnecessary drivers that don't pertain to your hardware.

## Configuration

hummingbird runs the init in three major stages, the filesystem stage, the interlude, and the tty stage.

Each stage should have execute permission in the filesystem.

#### Filesystem stage

hummingbird will mount all psuedo-filesystems required by the linux kernel for you. Once these filesystems have been mounted, hummingbird will then execute whatever is located at `/usr/lib/hummingbird/fs` (the filesystem stage). By the end of this stage, hummingbird expects that the root be remounted as RW (if it wasn't already), all necessary file systems have been mounted, and swap has been activated. Additionally, any filesystem integrity checks as well as filesystem decryption should be done during this stage as well.

After the filesystem stage, the hostname for the system is set. hummingbird will map the contents of `/etc/hostname` to memory and write them to the appropriate path to set the system's hostname. Once the hostname has been set hummingbird will seed the random number generator. The seed file is located at `/usr/lib/hummingbird/random.seed`. This file should be created during the building of hummingbird.

#### Interlude

The second stage is the interlude. This is where anything that is necessary to the system but is not a filesystem, nor is it related to spawning ttys is ran. This would include things like setting up hotplug for devices, running sysctl, or probing necessary modules for the system. By default, nothing exists at `/usr/lib/hummingbird/interlude` because hummingbird only does the minimum necessary to boot a system out of the box.

Additionally, a user may opt to setup wireless devices, acpi, or any other programs deemed necessary at boot.

#### TTY Stage

The final stage of hummingbird is the tty stage. This is solely to spawn TTYs for the system. In theory, this is the only mandatory file that needs to exist for hummingbird to successfully initialize a system.

Any TTY spawned in this stage should be forked to the background, but should also constantly respawn if killed. The script should block indefinitely to prevent hummingbird from dropping from initialization stages to an emergency shell.

## Emergency Shell

If for any reason during the system's boot time the init fails, hummingbird immediately drops you into an emergency shell. This shell is whatever is linked to `/bin/sh`.

While in the emergency shell, you have full root access to the system. From here you may edit the rc files to fix and repair the init as needed.

Once you have finished maintenance on the system, simply exit the emergency shell (`Ctrl+D`, `$ exit`, etc). hummingbird will immediately restart the system.

Note that there is a possibility for the system to boot-loop if the necessary initialization scripts do not exist (at the minimum the `tty` script) AND there is no system shell at `/bin/sh`.
