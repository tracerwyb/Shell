#pragma once
#include <string>
#include <vector>
class Shell
{
public:
    Shell();
    void prompt();
    void parse();
    void changeCurrentPath(std::vector<std::string> &args);
    void execute(std::vector<std::string> &args);
    void executeWithIO(std::vector<std::string> &args);
    void shell();

private:
    std::vector<std::vector<std::string>> argslist;
    std::string currentPath{"/"};
};
