/* torontocast-player/main.c */

#include <stdio.h>

#if defined(__linux__)
    #include <errno.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/ioctl.h>
    #include <unistd.h>

    #define WHICH_COMMAND_MAX_LENGTH   64
    #define  MAIN_COMMAND_MAX_LENGTH 2048

    #define IMG_PATH "/tmp/torontocast-album-cover.jpg"

    #define RADIO_URL      "https://kathy.torontocast.com"
    #define CURRENT_SONG   "api/v2/history/?limit=1&offset=0"
    #define JROCK_FRAGMENT "server=4"
    #define API_PORT       3310
    #define JROCK_PORT     3340


    static int check_dependencies(void);
    static int get_cell_size     (unsigned short *const, unsigned short *const);

    int main(void) {
        unsigned short cell_width;
        unsigned short cell_height;
        unsigned short img_size;
        unsigned short x_off;
        char           command[MAIN_COMMAND_MAX_LENGTH];

        /* check for bash shell */
        if (0 != system("which bash &> /dev/null")) {
            printf("\x1b[31merror:\x1b[0m `bash` shell is required\n");
            return 1;
        }

        /* check for binary dependencies */
        if (0 != check_dependencies()) {
            printf("\x1b[31merror:\x1b[0m not all dependencies are available\n");
            return 2;
        }

        /* calculate size for img2sixel */
        if (0 != get_cell_size(&cell_width, &cell_height)) {
            printf("\x1b[31merror:\x1b[0m failed to get terminal sizes\n");
            return 3;
        }
        img_size = cell_height * 3;
        x_off    = img_size / cell_width + 3;

        /* silence stdin echo */
        system("bash -c 'stty -echo -icanon'");

        /* immediately hide the cursor */
        printf("\x1b[?25l");
        fflush(stdout);

        sprintf(
            command,
            "bash -c '"
                /* launch the player in the background */
                "ffplay -i %s:%d -nodisp -fast -loglevel -8 &"
            "'",
            RADIO_URL, JROCK_PORT
        );
        system(command);

        sprintf(
            command,
            "bash -c '"
                /* forever until ^C */
                "while true; do "
                    /* get the json response about the current song from the API */
                    "response=$(curl --silent \"%s:%d/%s&%s\");"
                    /* extract the relevant fields */
                    "image=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"img_large_url\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "author=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"author\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "title=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"title\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "album=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"album\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "ts=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"ts\\\":\\K[^,]+\");"
                    "length=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"length\\\":\\K[^,]+\");"
                    /* draw album cover */
                    "curl -s -o %s \"${image}\";"
                    "img2sixel -w %hu -h %hu -I -S -d a_dither -r bilinear -q low -l disable -b xterm256 -E fast %s;"
                    "rm %s;"
                    /* print song info */
                    "echo -en \"\\x1b[2A\";"
                    "echo -en \"\\x1b[%huG\\x1b[34mauthor:\\x1b[0m ${author}\";"
                    "echo -en \"\\x1b[1B\";"
                    "echo -en \"\\x1b[%huG\\x1b[34mtitle:\\x1b[0m  ${title}\";"
                    "echo -en \"\\x1b[1B\";"
                    "echo -e  \"\\x1b[%huG\\x1b[34malbum:\\x1b[0m  ${album}\";"
                    /* print spaces to remove the black strip that sometimes shows in the sixel
                     * could also be fixed by using more precise arithmetic ig (short -> float/double)
                     */
                    "for i in {1..%hu}; do "
                        "echo -n \" \";"
                    "done;"
                    /* wait for next song (+4s because the metadata and audio stream are out of sync */
                    "sleep $((((${ts} + ${length}) - ($(date +%%s) * 1000)) / 1000 + 4));"
                    "echo;"
                "done"
            "'",
            RADIO_URL, API_PORT, CURRENT_SONG, JROCK_FRAGMENT,
            IMG_PATH, img_size, img_size, IMG_PATH, IMG_PATH,

            x_off, x_off, x_off,
            (unsigned short) (x_off - 2)
        );
        system(command);

        printf(
            "\x1b[1G"     /* go to the start of the previous line */
            "\x1b[?25h"  /* show the cursor                      */
        );

        /* restore stdin echo */
        system("bash -c 'stty icanon echo'");

        return 0;
    }

    static int check_dependencies(void) {
        static const char *const dependencies[] = {
            "curl", "date", "ffplay", "grep", "img2sixel", "rm", "sleep", "stty"
        };

        char command[WHICH_COMMAND_MAX_LENGTH];
        int  exitcode;
        int  ret = 0;

        unsigned char i   = 0;
        unsigned char len = sizeof(dependencies) / sizeof(dependencies[0]);
        for (; i < len; ++i) {
            sprintf(command, "bash -c 'which %s &> /dev/null'", dependencies[i]);
            exitcode = system(command);

            if (0 != exitcode) {
                printf("\x1b[31merror:\x1b[0m dependency `%s` is missing or not executable\n", dependencies[i]);
                ret = 1;
            }
        }

        return ret;
    }

    static int get_cell_size(unsigned short *const width, unsigned short *const height) {
        struct winsize ws;
        ws.ws_col    = 0;
        ws.ws_row    = 0;
        ws.ws_xpixel = 0;
        ws.ws_ypixel = 0;

        if (0 != ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)) {
            printf("\x1b[31merror:\x1b[0m `ioctl` returned \"%d: %s\"\n", errno, strerror(errno));
            return 1;
        }

        if (0 == (ws.ws_col * ws.ws_row * ws.ws_xpixel * ws.ws_ypixel)) {
            printf("\x1b[31merror:\x1b[0m one of the terminal sizes was invalid\n");
            return 2;
        }

        *width  = ws.ws_xpixel / ws.ws_col;
        *height = ws.ws_ypixel / ws.ws_row;

        return 0;
    }
#else
    int main(void) {
        printf("\x1b[31merror:\x1b[0m only linux is supported\n");
        return 1;
    }
#endif

