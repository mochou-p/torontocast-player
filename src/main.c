/* torontocast-player/main.c */

#include <stdio.h>

#if defined(__linux__)
    #include <errno.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/ioctl.h>
    #include <unistd.h>

    #define WHICH_COMMAND_LENGTH 64


    static int check_dependencies(void);
    static int get_cell_size     (unsigned short *, unsigned short *);

    int main(void) {
        unsigned short cell_width;
        unsigned short cell_height;
        unsigned short img_size;
        unsigned short x_off;
        char           command[2048];

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
        /* TODO: this will sometimes make a black strip in the sixel image,
         *       because unsigned short division (pixels/rows),
         *       use atleast float up until using it
         */
        img_size = cell_height * 3;
        x_off    = img_size / cell_width + 3;

        /* immediately hide the cursor */
        printf("\x1b[?25l");
        fflush(stdout);

        system(
            "bash -c '"
                /* launch the player in background */
                "ffplay -i https://kathy.torontocast.com:3340 -nodisp -fast -loglevel -8 &"
                /* save its PID */
                "echo $! > /tmp/torontocast-player-pid"
            "'"
        );

        sprintf(
            command,
            "bash -c '"
                /* get the json response about the current song from the API */
                "response=$(curl --silent \"https://kathy.torontocast.com:3310/api/v2/history/?limit=1&offset=0&server=4\");"
                /* extract the relevant fields */
                "image=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"img_large_url\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                "author=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"author\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                "title=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"title\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                "album=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"album\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                /* draw album cover */
                "img2sixel -w %hu -h %hu -I -S -d a_dither -r bilinear -q low -l disable -b xterm256 -E fast $image;"
                /* print song info */
                "echo -en \"\\x1b[2A\";"
                "echo -en \"\\x1b[%huG\\x1b[34mauthor:\\x1b[0m ${author}\";"
                "echo -en \"\\x1b[1B\";"
                "echo -en \"\\x1b[%huG\\x1b[34mtitle:\\x1b[0m  ${title}\";"
                "echo -en \"\\x1b[1B\";"
                "echo -e  \"\\x1b[%huG\\x1b[34malbum:\\x1b[0m  ${album}\""
            "'",
            img_size, img_size,
            x_off, x_off, x_off
        );
        system(command);

        /* TEMP */
        system("bash -c 'sleep 5'");

        system(
            "bash -c '"
                "kill $(< /tmp/torontocast-player-pid);"   /* stop the player process */
                "rm /tmp/torontocast-player-pid"          /* delete the PID file     */
            "'"
        );

        /* show the cursor */
        printf("\x1b[?25h");

        return 0;
    }

    static int check_dependencies(void) {
        static const char *const dependencies[] = {
            "curl", "ffplay", "grep", "img2sixel", "rm", "sleep", "stty"
        };

        char command[WHICH_COMMAND_LENGTH];
        int  exitcode;
        int  ret = 0;

        unsigned char i   = 0;
        unsigned char len = sizeof(dependencies) / sizeof(dependencies[0]);
        for (; i < len; ++i) {
            if (WHICH_COMMAND_LENGTH <= (strlen(dependencies[i]) + strlen("bash -c 'which  &> /dev/null'"))) {
                /* cause no snprintf in c89 */
                printf("\x1b[31mfatal:\x1b[0m saved from sprintf buffer overflow\n");
                return -1;
            }

            sprintf(command, "bash -c 'which %s &> /dev/null'", dependencies[i]);
            exitcode = system(command);

            if (0 != exitcode) {
                printf("\x1b[31merror:\x1b[0m dependency `%s` is missing or not executable\n", dependencies[i]);
                ret = 1;
            }
        }

        return ret;
    }

    static int get_cell_size(unsigned short *width, unsigned short *height) {
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

