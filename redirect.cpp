#include "redirect.h"
#include <signal.h>

Redirect::Redirect() {}

void Redirect::redio_by_pipe(std::vector<char **> argv)
{
    // std::cout << "redio_by_pipe\n";
    int pid0;
    int child_info = -1;

    if ((pid0 = fork()) == -1) {
        perror("fork");
    } else if (pid0 == 0) {
        int pid;

        if (pipe(apipe) == -1) {
            perror("could not make pipe\n");
            exit(1);
        }

        if ((pid = fork()) == -1) {
            perror("cannot fork\n");
            exit(1);
        }

        if (pid > 0) {       // Parent process
            close(apipe[0]); // Close read end of the pipe
            if (dup2(apipe[1], 1) == -1) {
                perror("dup 1 failed");
                exit(1);
            }
            close(apipe[1]); // Close the original pipe write descriptor

            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);

            execvp(argv[0][0], argv[0]);
            perror("execvp failed");
            exit(1);
        } else {             // Child process
            close(apipe[1]); // Close write end of the pipe
            if (dup2(apipe[0], 0) == -1) {
                perror("dup 0 failed");
                exit(1);
            }
            close(apipe[0]); // Close the original pipe read descriptor

            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);

            execvp(argv[1][0], argv[1]);
            perror("execvp failed");
            exit(1);
        }
    } else {
        if (wait(&child_info) == -1)
            perror("wait");
    }
}

void Redirect::reset_io()
{
    dup2(saved_stdin, fileno(stdin));
    dup2(saved_stdout, fileno(stdout));
}
