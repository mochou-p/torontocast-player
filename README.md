<!-- torontocast-player/README.md -->

# torontocast player
terminal-based internet radio player for [torontocast stations](https://asiadreamradio.torontocast.stream/stations/en/index.html)  
(currently only [J-Rock Powerplay](https://asiadreamradio.torontocast.stream/stations/jrockplayer.html))  

plays music in the background, and shows info about the current song  

![](https://i.imgur.com/ZOAp6z1.png)

## usage
build with `make` or `make debug`  

run with `make run` or `make run_debug`  
(alternatively just run the "torontocast-player[\_debug]" executable from the "bin" directory)  

exit by pressing [Ctrl]+\[C] (`^C`/SIGINT)  

clean directory with `make clean`  
(alternatively just do `rm -r bin`)

## requirements
| | |
|-:|:-|
| shell | [bash](https://www.gnu.org/software/make) (does **NOT** have to be the current shell) |
| runtime binaries | [Chafa](https://github.com/hpjansson/chafa) (and optionally [libsixel](https://github.com/libsixel/libsixel) if your [terminal supports it](https://www.arewesixelyet.com)), [coreutils](https://www.gnu.org/software/coreutils), [cURL](https://github.com/curl/curl), [FFmpeg](https://github.com/FFmpeg/FFmpeg) |
| build dependencies | [gcc](https://www.gnu.org/software/gcc), [make](https://www.gnu.org/software/make) |
| debugging tools | [Cppcheck](https://github.com/danmar/cppcheck), [Valgrind](https://valgrind.org) |

## notes
only tested on these distros: [Arch](https://archlinux.org), [NixOS](https://nixos.org)  

since exiting is currently done through SIGINT, running the program will most probably be the last thing that happens in your shell. this means that in this command: `make run; echo "hello"`, "hello" will not be printed  

most of the functionality is in bash, C is used for libc  

the screenshot uses:
- font: [MapleMono NF CN ExtraBold](https://github.com/subframe7536/maple-font)
- theme: [Catppuccin Mocha Blue](https://github.com/catppuccin/catppuccin)

## license
[GPL-3.0](./LICENSE)

