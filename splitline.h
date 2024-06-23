#pragma once
#include <iostream>

class Splitline
{
public:
    Splitline();
    Splitline(char* sline);
    char** splitline();
    char* next_cmd(char* prompt, FILE* fp);
    void* erealloc(void* p, size_t n);
    void* emalloc(size_t n);
    void freelist(char** list);
    char* newstr(char* s, int l);
    void setline(char* line);

private:
    char* m_line;
};
