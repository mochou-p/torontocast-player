/* torontocast-player/main.c */

#include <stdio.h>

#if defined(__linux__)
    #include <stdlib.h>
    #include <string.h>

    #define WHICH_COMMAND_LENGTH 64


    int check_dependencies(void);

    int main(void) {
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

        printf(
            "\x1b[?1049h"     /* enter alternative screen    */
            "\x1b[?25l"      /* hide the cursor             */
            "\x1b[2J"       /* clear the whole screen      */
            "\x1b[1;1H"    /* move the cursor to top left */
        );
        fflush(stdout);

        system(
            "bash -c '"
                /* launch the player in background */
                "ffplay -i https://kathy.torontocast.com:3340 -nodisp -fast -loglevel -8 &"
                /* save its PID */
                "echo $! > /tmp/torontocast-player-pid"
            "'"
        );

        system(
            "bash -c '"
                /* get the json response about the current song from the API */
                "response=$(curl --silent \"https://kathy.torontocast.com:3310/api/v2/history/?limit=1&offset=0&server=4\");"
                /* extract the relevant fields */
                "author=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"author\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                "title=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"title\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                "album=$(echo -n \"${response}\" | grep -m 1 -oP \"\\\"album\\\":\\\"\\K[^\\\"]+(?=\\\"\\,\\\")\");"
                /* print them */
                "echo -e \"\\x1b[34mauthor:\\x1b[0m ${author}\";"
                "echo -e \"\\x1b[34mtitle:\\x1b[0m  ${title}\";"
                "echo -e \"\\x1b[34malbum:\\x1b[0m  ${album}\""
            "'"
        );

        /* TEMP */
        system("bash -c 'sleep 5'");

        system(
            "bash -c '"
                "kill $(< /tmp/torontocast-player-pid);"   /* stop the player process */
                "rm /tmp/torontocast-player-pid"          /* delete the PID file     */
            "'"
        );

        printf(
            "\x1b[?1049l"   /* leave anternative screen */
            "\x1b[?25h"    /* show the cursor          */
        );

        return 0;
    }

    int check_dependencies(void) {
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
                printf("\x1b[31mfatal:\x1b[0m saved from buffer overflow\n");
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
#else
    int main(void) {
        printf("\x1b[31merror:\x1b[0m only linux is supported\n");
        return 1;
    }
#endif

