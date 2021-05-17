<div align="center">
<img src="hummingbird.png" width="350px">
<p>Lightning fast Linux init</p>
<a href="https://travis-ci.org/Sweets/hummingbird">
    <img src="https://travis-ci.org/Sweets/hummingbird.svg?branch=master">
</a>
<a href="LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-blue.svg">
</a>
<img src="https://img.shields.io/badge/dependencies-stdlib-navy.svg">
<img src="https://img.shields.io/badge/platform-Linux-lightgray.svg">
<img src="https://img.shields.io/github/repo-size/Sweets/hummingbird.svg?color=green">
</div>

"<i>It makes runit look like systemd</i>" —<a href="https://github.com/wooosh/">wooosh</a><br/>
"<i>the antithesis of systemd</i>" —<a href="https://www.reddit.com/r/linux/comments/ndnwzx/hummingbird_a_lightning_fast_linux_init/gyegkkd/">u/ocean-noice</a>

# about

hummingbird is an init system designed for speed.
It does nothing more than start the system and stop the system by default. There are three main "runlevels", `fs` which is called directly after the pseudo filesystems necessary for the system to run have been mounted, `interlude` which is ran before dropping the user into a tty, and `tty` which is what is responsible for actually starting ttys.

# installation

```
$ git clone https://github.com/Sweets/hummingbird
$ cd ./hummingbird
$ make
$ sudo make install
```

### additional files

If your distribution has any files specific to it, they can be found in `dist`. Replace everything in `/usr/lib/hummingbird` with your distribution specific files for optimal usage with your distribution.

If they're not available, feel free to contribute them, or request them.

