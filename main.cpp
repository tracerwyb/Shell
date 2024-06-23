#include "execute.h"
#include "splitline.h"
#include <iostream>

#define DFL_PROMPT "> "
int main()
{
    char* cmdline;
    char** arglist;
    char* prompt = DFL_PROMPT;
    Execute ec;
    Splitline sl;
    while ((cmdline = sl.next_cmd(prompt, stdin)) != NULL) {
        sl.setline(cmdline);
        if ((arglist = sl.splitline()) != NULL) {
            ec.execute(arglist);
        }
        free(cmdline);
    }
    return 0;
}
