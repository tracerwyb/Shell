#pragma once
#include "unistd.h"
#include <iostream>
#include <sys/wait.h>
#include <vector>

class Redirect
{
public:
    Redirect();
    void redio_by_pipe(std::vector<char**> argv);
    void reset_io();

private:
    FILE* m_newinput;
    FILE* m_newoutput;
    int apipe[2];
    int saved_stdin = dup(fileno(stdin));
    int saved_stdout = dup(fileno(stdout));
};
