#include "shell.h"
#include <cstdlib>
#include <fcntl.h> // 引入open, O_RDONLY等
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
Shell::Shell()
{
    shell();
}

void Shell::prompt()
{
    std::string username = getenv("USER");
    struct utsname uts;
    uname(&uts);
    std::string hostname(uts.nodename);
    std::string prompt{"[" + username + " " + hostname + "~" + currentPath + "]~"};
    // std::string prompt{"[" + username + " " + hostname + "]<"};
    std::cout << prompt;
}

void Shell::parse()
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
    // std::cout << cmdline << std::endl;
    int i = 0;
    std::string tmp;
    std::vector<std::string> args;
    std::string filename{};
    while (i < cmdline.size()) {
        // std::cout << "start" << cmdline[i] << std::endl;
        if (cmdline[i] == '|' || cmdline[i] == ';') {
            if (!tmp.empty()) { // 如果tmp不为空，则添加它到参数列表中
                args.push_back(tmp);
                tmp.clear();
            }
            if (!filename.empty()) {
                args.push_back(filename);
                args.push_back("f");
                filename.clear();
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
            if (!filename.empty()) {
                args.push_back(filename);
                args.push_back("f");
                filename.clear();
            }
            //输入输出的重定向和管道
        } else if (cmdline[i] == '>') {
            i++;
            if (cmdline[i] == '>') {
                i++;
            } else if (cmdline[i + 1] == '>') {
                std::cerr << "some error occur in <<";
                exit(0);
            }
            //重定向符号后内容
            int flag = 0;
            while (i < cmdline.size()) {
                if (cmdline[i] == ' ' && flag == 0) {
                    i++;
                } else if (cmdline[i] == ' ' && flag == 1) {
                    break;
                } else {
                    flag = 1;
                    filename = filename + cmdline[i];
                    i++;
                }
            }
            flag = 0;
            i--;
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
    if (!filename.empty()) {
        args.push_back(filename);
        args.push_back("f");
    }
    // 最后一个参数列表不为空，则添加到argslist中
    if (!args.empty()) {
        argslist.push_back(args);
    }
    for (auto &arg : argslist) {
        // for (auto &a : arg) {
        //     std::cout << ":" << a;
        // }
    }
}

void Shell::changeCurrentPath(std::vector<std::string> &args)
{
    // 上一个文件目录
    if (args[1] == "../") {
        if (currentPath == "/") {
            ;
        } else {
            size_t pos = currentPath.rfind('/');
            if (pos != std::string::npos) {
                currentPath.erase(pos + 1);
            }
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

void Shell::execute(std::vector<std::string> &args)
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

void Shell::executeWithIO(std::vector<std::string> &args)
{
    // std::cout << "chongdingxiang" << std::endl;
    char *arr[10];
    std::string filename = args[args.size() - 2] + ".txt";
    if (args.size() >= 2) {
        args.pop_back();
        if (!args.empty()) {
            args.pop_back();
        }
    }

    pid_t pid;
    //create child process
    pid = fork();
    if (pid == -1) {
        std::cout << "fork error" << std::endl;
        exit(1);
    } else if (pid == 0) {
        //child process
        int fd;
        close(1);
        filename = currentPath + filename;
        fd = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
        if (fd != 1) {
            fprintf(stderr, "Could not open sata as fd 1\n");
            exit(1);
        }
        if (args.size() == 1) {
            args.push_back(currentPath);
        }
        for (int i = 0; i < args.size(); i++) {
            arr[i] = (char *) args[i].c_str();
        }
        arr[args.size()] = NULL;
        execvp(arr[0], arr);
        close(fd);
    }
    //parent process
    else {
        wait(NULL);
    }
}

void Shell::shell()
{
    int count = 0;
    while (true) {
        prompt();
        parse();
        if (count == 20) {
            break;
        }
        count++;
        for (auto &args : argslist) {
            if (args.size() > 0 && args[0] == "cd") {
                changeCurrentPath(args);
            } else if (args[args.size() - 1] == "f") {
                executeWithIO(args);
            } else {
                execute(args);
            }
        }
        argslist.clear();
    }
}
