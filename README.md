<!-- torontocast-player/README.md -->

# torontocast player
terminal-based internet radio player for [torontocast stations](https://asiadreamradio.torontocast.stream/stations/en/index.html) (currently only [J-Rock Powerplay](https://asiadreamradio.torontocast.stream/stations/jrockplayer.html))  
![](https://i.imgur.com/ZOAp6z1.png)

## usage
build with `make`/`make debug`  

run with `make run`/`make run_debug`  
(or just run the "torontocast-player[\_debug]" executable from bin/)  

exit by pressing [Ctrl]+\[C] (or send `^C`/SIGINT some other way)

## requirements
| | |
|-:|:-|
| OS | GNU/Linux |
| terminal | [one that supports sixel graphics](https://www.arewesixelyet.com) |
| shell | [bash](https://www.gnu.org/software/make) (does **NOT** have to be the current shell) |
| runtime binaries | [coreutils](https://www.gnu.org/software/coreutils), [cURL](https://github.com/curl/curl), [FFmpeg](https://github.com/FFmpeg/FFmpeg), [libsixel](https://github.com/libsixel/libsixel)) |
| build dependencies | [gcc](https://www.gnu.org/software/gcc), [make](https://www.gnu.org/software/make) |

## notes
most of the functionality is in bash, C is used for libc  
the screenshot uses:
- terminal: [foot](https://codeberg.org/dnkl/foot)
- font: [MapleMono NF CN ExtraBold](https://github.com/subframe7536/maple-font)
- theme: [Catppuccin Mocha Blue](https://github.com/catppuccin/catppuccin)

## license
[GPL-3.0](./LICENSE)

