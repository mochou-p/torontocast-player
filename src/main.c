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
        unsigned short x_off;
        char           command[MAIN_COMMAND_MAX_LENGTH];

        /* check for bash shell */
        if (0 != system("which bash &> /dev/null")) {
            printf("\x1b[31merror:\x1b[0m `bash` shell is required\n");
            return 2;
        }

        /* check for binary dependencies */
        if (0 != check_dependencies()) {
            printf("\x1b[31merror:\x1b[0m not all dependencies are available\n");
            return 3;
        }

        /* calculate horizontal offset for current font */
        if (0 != get_cell_size(&cell_width, &cell_height)) {
            printf("\x1b[31merror:\x1b[0m failed to get terminal sizes\n");
            return 4;
        }
        x_off = (unsigned short) (cell_height * 3 / cell_width + 3);

        /* immediately hide the cursor */
        printf("\x1b[?25l");
        fflush(stdout);

        /* silence stdin echo */
        system("bash -c 'stty -echo -icanon'");

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
                    "image=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"img_url\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "author=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"author\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "title=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"title\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "album=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"album\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                    "ts=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"ts\\\":\\K[^,]+\");"
                    "length=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"length\\\":\\K[^,]+\");"
                    /* download the album cover */
                    "curl -s -o %s \"${image}\";"
                    /* draw it
                     * big width because it will get clamped to match the height, no matter the font size dimensions ratio
                     */
                    "chafa --symbols=all -O 0 --relative=on --margin-bottom=0 --size=1000x3 --animate=off %s;"
                    /* chafa showed the cursor, hide it again */
                    "echo -en \"\\x1b[?25l\";"
                    /* delete it */
                    "rm %s;"
                    /* print song info */
                    "echo -en \"\\x1b[3A\";"
                    "echo -en \"\\x1b[%huG\\x1b[34mauthor:\\x1b[0m ${author}\";"
                    "echo -en \"\\x1b[1B\";"
                    "echo -en \"\\x1b[%huG\\x1b[34mtitle:\\x1b[0m  ${title}\";"
                    "echo -en \"\\x1b[1B\";"
                    "echo -e  \"\\x1b[%huG\\x1b[34malbum:\\x1b[0m  ${album}\";"
                    /* wait for next song (+4s because the metadata and audio stream are out of sync */
                    "sleep $((((${ts} + ${length}) - ($(date +%%s) * 1000)) / 1000 + 4));"
                    "echo;"
                "done"
            "'",
            RADIO_URL, API_PORT, CURRENT_SONG, JROCK_FRAGMENT,
            IMG_PATH, IMG_PATH, IMG_PATH,
            x_off, x_off, x_off
        );
        system(command);

        /* restore stdin echo */
        system("bash -c 'stty icanon echo'");

        printf(
            "\x1b[1G"     /* go to the start of the previous line */
            "\x1b[?25h"  /* show the cursor                      */
        );

        /* close inherited pseudo-ttys */
        fclose(stdin );
        fclose(stdout);
        fclose(stderr);

        return 0;
    }

    static int check_dependencies(void) {
        static const char *const dependencies[] = {
            "chafa", "curl", "date", "ffplay", "grep", "rm", "sleep", "stty"
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

