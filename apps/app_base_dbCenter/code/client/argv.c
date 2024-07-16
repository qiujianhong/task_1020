#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define CFG_MAXARGS 200

int parse_argv(char *str, int *argc, char **argv, int number)
{
    char *p;
    int   num        = 0;
    int   word_start = 1;

    if (argc == NULL || argv == NULL)
        return -1;

    p = str;

    while (*p)
    {
        if ((*p == '\r') || (*p == '\n'))
        {
            *p = '\0';
            break;
        }
        if ((*p == ' ') || (*p == '\t'))
        {
            word_start = 1;
            *p         = '\0';
            p++;
            continue;
        }
        if (num >= number)
            break;

        if (word_start)
        {
            argv[num++] = p;
            word_start  = 0;
        }
        p++;
    }

    *argc = num;

    return 0;
}

int parse_line(char *line, char *argv[])
{
    int nargs = 0;

#ifdef DEBUG_PARSER
    printf("parse_line: \"%s\"\n", line);
#endif
    while (nargs < CFG_MAXARGS)
    {
        /* skip any white space */
        while ((*line == ' ') || (*line == '\t'))
        {
            ++line;
        }
        
        if (*line == '\0')
        { /* end of line, no more args */
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
             printf("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }

        argv[nargs++] = line; /* begin of argument string */

        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        { /* end of line, no more args */
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
             printf("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }

        *line++ = '\0';
        /* terminate current arg */
    }

    printf("** Too many args (max. %d) **\n", CFG_MAXARGS);

#ifdef DEBUG_PARSER
    printf("parse_line: nargs=%d\n", nargs);
#endif
    return (nargs);
}