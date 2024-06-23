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
        sl.add_cmd(cmdline);
        if (sl.isnextline()) {
            ec.execute(sl.split(), &sl);
            sl.reset_mlines();
        }
        free(cmdline);
    }
    std::cout << "quit while\n";
    return 0;
}
