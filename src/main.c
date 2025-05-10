/* torontocast-player/main.c */

#include <stdio.h>

#if defined(__linux__)
    #include <stdlib.h>
    #include <string.h>

    #define WHICH_COMMAND_LENGTH 64


    int check_dependencies(void);

    int main(void) {
        if (0 != system("which bash &> /dev/null")) {
            printf("\x1b[31merror:\x1b[0m `bash` shell is required\n");
            return 1;
        }

        if (0 != check_dependencies()) {
            printf("\x1b[31merror:\x1b[0m not all dependencies are available\n");
            return 2;
        }

        printf("works\n");

        return 0;
    }

    int check_dependencies(void) {
        static const char *const dependencies[] = {
            "curl", "ffplay", "grep", "img2sixel"
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

