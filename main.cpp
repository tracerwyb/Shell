#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
std::vector<std::vector<std::string>> argslist;
std::string currentPath{"/"};

//include nusername ,currentfile,hostname,a prompt compose total prompt
void prompt()
{
    std::string username = getenv("USER");
    struct utsname uts;
    uname(&uts);
    std::string hostname(uts.nodename);
    std::string prompt{"[" + username + " " + hostname + "~" + currentPath + "]<"};
    // std::string prompt{"[" + username + " " + hostname + "]<"};
    std::cout << prompt;
}

void parse()
{
    std::string cmdline;
    std::getline(std::cin, cmdline); // 使用std::getline从标准输入读取一行文本
    //除去开头空格
    int pos = 0;
    while (pos < cmdline.size() && std::isspace(cmdline[pos])) {
        ++pos;
    }
    if (pos > 0) {
        cmdline.erase(0, pos);
    }
    int i = 0;
    std::string tmp;
    std::vector<std::string> args;
    while (i < cmdline.size()) {
        if (cmdline[i] == '|' || cmdline[i] == ';') {
            if (!tmp.empty()) { // 如果tmp不为空，则添加它到参数列表中
                args.push_back(tmp);
                tmp.clear();
            }
            argslist.push_back(args); // 添加参数列表
            args.clear();             // 清空当前参数列表
            i++;
            continue;
        } else if (cmdline[i] == ' ') {
            if (!tmp.empty()) {
                args.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp = tmp + cmdline[i];
        }
        i++;
    }
    //---------------以下针对ls 也就是无;|的情况
    // 最后一个参数
    if (!tmp.empty()) {
        args.push_back(tmp);
    }
    // 最后一个参数列表不为空，则添加到argslist中
    if (!args.empty()) {
        argslist.push_back(args);
    }
}
//cd
void changeCurrentPath(std::vector<std::string> &args)
{
    // 上一个文件目录
    if (args[1] == "../") {
        size_t pos = currentPath.rfind('/');
        if (pos != std::string::npos) {
            currentPath.erase(pos + 1);
        }
    } else if (args[1] == "./") {
        ;
    } else {
        // 判断系统里是否存在这个目录
        if (currentPath.back() == '/') {
            if (!args[1].empty() && args[1][0] == '/') {
                args[1] = args[1].substr(1); // 返回从索引1（即第二个字符）开始的子字符串
            }
        }
        std::string fullPath = currentPath + args[1];
        if (std::filesystem::exists(fullPath) && std::filesystem::is_directory(fullPath)) {
            currentPath = fullPath;
        } else {
            std::cerr << "该目录不存在: " << fullPath << std::endl;
        }
    }
}
void execute(std::vector<std::string> &args)
{
    char *arr[10];
    pid_t pid;
    //create child process
    pid = fork();
    if (pid == -1) {
        std::cout << "fork error" << std::endl;
        exit(1);
    } else if (pid == 0) {
        //child process
        if (args.size() == 1) {
            args.push_back(currentPath);
        }
        for (int i = 0; i < args.size(); i++) {
            arr[i] = (char *) args[i].c_str();
        }
        arr[args.size()] = NULL;
        execvp(arr[0], arr);
    }
    //parent process
    else {
        wait(NULL);
    }
}

void shell()
{
    while (true) {
        prompt();
        parse();
        for (auto &args : argslist) {
            if (args.size() > 0 && args[0] == "cd") {
                changeCurrentPath(args);
            } else {
                execute(args);
            }
        }
        argslist.clear();
    }
}
int main()
{
    shell();
    return 0;
}
