<!-- torontocast-player/README.md -->

# torontocast player
terminal-based internet radio player for [torontocast stations](https://asiadreamradio.torontocast.stream/stations/en/index.html)  
most of the functionality uses bash, C is used for libc

## usage
build with `make`/`make debug`  
run with `make run`/`make run_debug`

## requirements
|                    |                               |
|-------------------:|:------------------------------|
|                 OS | Linux                         |
|              shell | bash                          |
|   runtime binaries | curl, ffplay, grep, img2sixel |
| build dependencies | gcc, make                     |

## license
[GPL-3.0](./LICENSE)

