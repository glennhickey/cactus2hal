/*
 * HALparser.cpp
 *
 *  Created on: May 2, 2012
 *      Author: Vlado Uzunangelov
 *     parseOptions code has been copied from
 *     Dent Earl's MAFTools library.
 */

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void parseOptions(int argc, char **argv, char *seqName, uint32_t *position) {
    int c;
    int setSName = 0, setPos = 0;
    int32_t tempPos = 0;
    while (1) {
        static struct option long_options[] = {
            //{"debug", no_argument, &debug_flag, 1},
            //{"verbose", no_argument, 0, 'v'},
            //{"help", no_argument, 0, 'h'},
            {"seq",  required_argument, 0, 's'},
            {"pos",  required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "n:c:p:v",
                        long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 0:l
            break;
        case 's':
            setSName = 1;
            sscanf(optarg, "%s", seqName);
            break;
        case 'p':
            setPos = 1;
            tempPos = strtoll(optarg, NULL, 10);
            if (tempPos < 0) {
                fprintf(stderr, "Error, --pos %d must be nonnegative.\n", tempPos);

            }
            *position = tempPos;
            break;
        case 'v':
            verbose_flag++;
            break;
        case 'h':
        case '?':
            break;
        default:
            abort();
        }
    }
    if (!(setSName && setPos)) {
        fprintf(stderr, "specify --seq --position\n");
    }
    // Check there's nothing left over on the command line
    if (optind < argc) {
        char errorString[30] = "Unexpected arguments:";
        while (optind < argc) {
            strcat(errorString, " ");
            strcat(errorString, argv[optind++]);
        }
        fprintf(stderr, "%s\n", errorString);
    }
}


int main(int argc, char *argv[]){

}


