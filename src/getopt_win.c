#include "getopt_win.h"
#include <stdio.h>
#include <string.h>

char *optarg;
int optind = 1, opterr = 1, optopt;

int getopt(int argc, char *const argv[], const char *optstring) {
    static int optpos = 1;
    if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
        return -1;
    }
    optopt = argv[optind][optpos];
    const char *opt = strchr(optstring, optopt);
    if (!opt) {
        if (opterr) {
            fprintf(stderr, "Unknown option: -%c\n", optopt);
        }
        return '?';
    }
    if (opt[1] == ':') {
        if (argv[optind][optpos + 1] != '\0') {
            optarg = &argv[optind][optpos + 1];
            optind++;
        } else if (optind + 1 < argc) {
            optarg = argv[optind + 1];
            optind += 2;
        } else {
            if (opterr) {
                fprintf(stderr, "Option -%c requires an argument\n", optopt);
            }
            return '?';
        }
        optpos = 1;
    } else {
        if (argv[optind][++optpos] == '\0') {
            optpos = 1;
            optind++;
        }
    }
    return optopt;
}

int getopt_long(int argc, char *const argv[],
                const char *optstring,
                const struct option *longopts, int *longindex) {
    /* If the current argument does not start with '-' return -1 */
    if (optind >= argc || argv[optind][0] != '-') {
        return -1;
    }

    /* Check for long option marker: -- */
    if (argv[optind][1] == '-') {
        char *current_arg = argv[optind] + 2;  // Skip "--"
        char *equal_sign = strchr(current_arg, '=');
        size_t option_len = equal_sign ? (size_t)(equal_sign - current_arg) : strlen(current_arg);
        int i, matched = -1;
        for (i = 0; longopts[i].name != NULL; i++) {
            if (strncmp(current_arg, longopts[i].name, option_len) == 0 &&
                strlen(longopts[i].name) == option_len) {
                matched = i;
                break;
            }
        }
        if (matched == -1) {
            if (opterr) {
                fprintf(stderr, "Unknown option '--%s'\n", current_arg);
            }
            optind++;
            return '?';
        }
        if (longindex != NULL) {
            *longindex = matched;
        }
        if (longopts[matched].flag != NULL) {
            *(longopts[matched].flag) = longopts[matched].val;
            /* If an argument is required or optional, process it */
            if (longopts[matched].has_arg != no_argument) {
                if (equal_sign != NULL) {
                    optarg = equal_sign + 1;
                } else if (optind + 1 < argc) {
                    optarg = argv[++optind];
                } else {
                    if (opterr) {
                        fprintf(stderr, "Option '--%s' requires an argument\n", longopts[matched].name);
                    }
                    optind++;
                    return '?';
                }
            }
            optind++;
            return 0;
        } else {
            int ret_val = longopts[matched].val;
            if (longopts[matched].has_arg != no_argument) {
                if (equal_sign != NULL) {
                    optarg = equal_sign + 1;
                } else if (optind + 1 < argc) {
                    optarg = argv[++optind];
                } else {
                    if (opterr) {
                        fprintf(stderr, "Option '--%s' requires an argument\n", longopts[matched].name);
                    }
                    optind++;
                    return '?';
                }
            }
            optind++;
            return ret_val;
        }
    } else {
        /* Not a long option; fall back to getopt */
        return getopt(argc, argv, optstring);
    }
}
