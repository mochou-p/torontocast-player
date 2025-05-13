<!-- torontocast-player/README.md -->

# torontocast player
terminal-based internet radio player for [torontocast stations](https://asiadreamradio.torontocast.stream/stations/en/index.html)  
(currently only [J-Rock Powerplay](https://asiadreamradio.torontocast.stream/stations/jrockplayer.html))  

plays music in the background, and shows info about the current song  

![](https://i.imgur.com/SnGv0YL.png)

## usage
| action | command | description |
|:-|:-|:-:|
| build | `make` | compiles the project into the "bin/" directory |
| run | `make run` | - |
| exit | by pressing \[Ctrl]+\[C] | - |
| install | `make install` | allows running this from any path like a global command<br>(no "/usr/bin/" on NixOS, you can make a shell alias instead) |
| uninstall | `make uninstall` | - |
| clean | `make clean` | removes all executables and build/debugging artifacts |

## requirements
- runtime binaries
    - [Chafa](https://github.com/hpjansson/chafa) for terminal graphics (and optionally [libsixel](https://github.com/libsixel/libsixel) if your [terminal supports it](https://arewesixelyet.com))
    - [cURL](https://github.com/curl/curl) for networking
    - [FFmpeg](https://github.com/FFmpeg/FFmpeg) for playing music (`ffplay`)
    - GNU software (most Linux distros should already have these)
        - [bash](https://gnu.org/software/bash) (does **NOT** have to be the current shell)
        - [coreutils](https://gnu.org/software/coreutils)
        - [grep](https://gnu.org/software/grep) for API response parsing
        - [which](https://gnu.org/software/which) for runtime checks of available dependencies
- build dependencies
    - [gcc](https://gnu.org/software/gcc) (C compiler)
    - [make](https://gnu.org/software/make) (build system)
    - [sudo](https://github.com/sudo-project/sudo) for accessing "/usr/bin/"
    - optional debugging tools
        - [Cppcheck](https://github.com/danmar/cppcheck) (static code analyzer)
        - [Valgrind](https://valgrind.org) (memory-management problem detection)

## notes
tested on these Linux distros: [Arch](https://archlinux.org), [NixOS](https://nixos.org)  
macOS could work, feel free to contribute!  
Windows support is not planned  

since exiting is currently done through SIGINT, running the program will most probably be the last thing that happens in your shell. this means that in this command: `make run; echo "hello"`, "hello" will not be printed  

most of the functionality is in bash, C is used for libc  

the screenshot uses:
- font: [MapleMono NF CN ExtraBold](https://github.com/subframe7536/maple-font)
- theme: [Catppuccin Mocha Blue](https://github.com/catppuccin/catppuccin)

## license
[GPL-3.0](./LICENSE)

