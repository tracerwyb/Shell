#include "execute.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

Execute::Execute()
    : m_rd{new Redirect()}
{}

void Execute::execute(std::vector<char **> args, Splitline *sl)
{
    if (!sl->ispipe()) {
        for (auto i : args)
            if (i != NULL)
                execute_one(i);
    } else {
        sl->reset_ispipe();
        m_rd->redio_by_pipe(args);
    }
}

void Execute::execute_one(char *argv[])
{
    int pid;
    int child_info = -1;
    if ((pid = fork()) == -1) {
        perror("fork");
    } else if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        execvp(argv[0], argv);
        perror("cannot execute command");
        exit(1);
    } else {
        if (wait(&child_info) == -1)
            perror("wait");
    }
}
