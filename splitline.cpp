#include "splitline.h"
#include "redirect.h"
#include <string.h>

#define is_delim(x) ((x) == ' ' || (x) == '\t')

Splitline::Splitline() {}

void Splitline::add_cmd(char* line)
{
    char* line_tmp = (char*) emalloc(sizeof(line));
    strcpy(line_tmp, line);
    m_lines.push_back(line_tmp);
}

bool Splitline::isnextline()
{
    return m_isnextline;
}

void Splitline::reset_isnextline()
{
    m_isnextline = true;
}

bool Splitline::ispipe()
{
    return m_ispipe;
}

void Splitline::reset_ispipe()
{
    m_ispipe = false;
}

void Splitline::reset_mlines()
{
    std::vector<char*>().swap(m_lines);
}

std::vector<char**> Splitline::split()
{
    std::vector<char**> args;
    for (auto line : m_lines) {
        args.push_back(splitline(line));
        // std::cout << "line" << line << '\n';
    }
    return args;
}

char** Splitline::splitline(char* line)
{
    if (line != NULL) {
        int spots = 0;         // 最大参数个数
        int argnum = 0;        // 实际参数个数
        int bufspace = BUFSIZ; //初始缓冲区大小
        char** args;           // 参数数组
        char* cp = line;
        char* start;
        int len;

        args = (char**) emalloc(BUFSIZ);
        spots = BUFSIZ / sizeof(char*); //初始设置的最大参数个数
        while (*cp != '\0') {
            while (is_delim(*cp))
                cp++;
            if (*cp == '\0') /* quit at end-o-string	*/
                break;

            // 若初始设置的缓冲区大小不够
            if (argnum + 1 >= spots) {
                args = (char**) erealloc(args, bufspace + BUFSIZ);
                bufspace += BUFSIZ;
            }

            start = cp;
            len = 1;
            while (*++cp != '\0' && !(is_delim(*cp))) {
                len++;
            }
            args[argnum++] = newstr(start, len); // 截取参数,并以'\0'结尾
        }
        args[argnum] = NULL;
        return args;
    }
    return NULL;
}

char* Splitline::next_cmd(char* prompt, FILE* fp)
{
    m_isnextline = false;

    char* buf;
    int bufspace = 0;
    int pos = 0;
    int c;

    std::cout << prompt;
    while ((c = getc(fp)) != EOF) {
        // allocate new space

        if (pos + 1 > bufspace) {
            if (bufspace == 0) {
                buf = (char*) emalloc(BUFSIZ);
            } else {
                buf = (char*) erealloc(buf, bufspace + BUFSIZ);
            }
            bufspace += BUFSIZ;
        }
        if (c == '\n' || c == ';' || c == '|') {
            // Redirect rd;
            if (c == '\n') {
                reset_isnextline();
                // reset_ispipe();
            }
            if (c == '|') {
                m_ispipe = true;
                // rd.redio_by_pipe();
            }
            if (c == ';') {
                // rd.reset_io();
            }

            break;
        }

        // add to buffer
        buf[pos++] = c;
    }
    if (c == EOF && pos == 0) {
        return NULL;
    }
    buf[pos] = '\0';
    return buf;
}

void Splitline::freelist(char** list)
{
    char** cp = list;
    while (*cp)
        free(*cp++);
    free(list);
}

char* Splitline::newstr(char* s, int l)
{
    char* rv = (char*) emalloc(l + 1);
    strncpy(rv, s, l);
    rv[l] = '\0';
    return rv;
}

void* Splitline::emalloc(size_t n)
{
    void* rv;
    if ((rv = malloc(n)) == NULL)
        perror("out of memory");
    return rv;
}

void* Splitline::erealloc(void* p, size_t n)
{
    void* rv;
    if ((rv = realloc(p, n)) == NULL)
        perror("realloc() failed");
    return rv;
}
