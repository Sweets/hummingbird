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
"<i>the antithesis of systemd</i>" —<a href="https://www.reddit.com/r/linux/comments/ndnwzx/hummingbird_a_lightning_fast_linux_init/gyegkkd/">u/ocean-noice</a><br/>
<a href="https://nosystemd.org">No systemd</a>

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

# supported compilers

Hummingbird is known to build with the following C compilers:

- [GNU Compiler Collection (GCC)](https://gcc.gnu.org/)
- [Clang](https://clang.llvm.org/)
- [cproc](https://sr.ht/~mcf/cproc/)
- [Kefir](https://git.sr.ht/~jprotopopov/kefir)
- [Tiny C Compiler (TCC)](https://www.bellard.org/tcc/)

Building with a compiler not listed here? Add it and send a pull request!

# work on hummingbird

As a developer, I feel that the functionality of hummingbird has reached what I envisioned it to. This is not to say that the project is dead by any means — I simply have nothing more to add to it. That being said though, if you ever feel there's an issue with hummingbird, open an issue. It will be addressed as soon as I can get to it.

Don't ever be intimidated by the last commit date being a long time in the past. If you see I've made commits on other projects in the last year, rest assured I'll get to your issue.

# usage

Feel free to use hummingbird on your own personal installation, on any sort of embedded system you wish, hell, if you're a distribution developer, feel free to ship it as the init if you're so inclined. hummingbird is licensed under the MIT license — the world is your oyster. If you have any cool projects that use hummingbird, feel free to make a pull request or an issue showing how it's being used, it'll be listed on the repository README.
