#pragma once
#include "redirect.h"
#include "splitline.h"
#include <iostream>
#include <string.h>
#include <vector>

class Execute
{
public:
    Execute();
    void execute_one(char *argv[]);
    void execute(std::vector<char **> argv, Splitline *sl);

private:
    Redirect *m_rd;
};
