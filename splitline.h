#pragma once
#include <iostream>
#include <vector>

class Splitline
{
public:
    Splitline();
    char* next_cmd(char* prompt, FILE* fp);
    std::vector<char**> split();
    char** splitline(char* line);
    void* erealloc(void* p, size_t n);
    void* emalloc(size_t n);
    void freelist(char** list);
    char* newstr(char* s, int l);
    void add_cmd(char* line);
    bool isnextline();
    void reset_isnextline();
    bool ispipe();
    void reset_ispipe();
    void reset_mlines();

private:
    std::vector<char*> m_lines;
    bool m_isnextline = true;
    bool m_ispipe = false;
};
