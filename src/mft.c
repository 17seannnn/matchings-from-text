#include <stdio.h>
#include <stdlib.h>

enum { patterns_buffer_size = 32,
       word_buffer_size = 256,
       text_buffer_size = 131072,
       star_replace = 1, question_replace = 2, empty_replace = 3,
       status_stop = 0, status_read = 1, status_linebreak = 2 };

int string_length(const char *str)
{
    const char *tmp;
    for(tmp = str; *tmp; tmp++)
        {}
    return tmp - str;
}

void string_copy(const char *src, char *dst)
{
    for(; *src; src++, dst++)
        *dst = *src;
    *dst = 0;
}

int string_compare(const char *cmp1, const char *cmp2)
{
    for(; *cmp1; cmp1++, cmp2++) {
        if(*cmp1 != *cmp2)
            return 0;
    }
    return 1;
}

int match(const char *str, const char *ptr)
{
    for(;; str++, ptr++) {
        switch(*ptr) {
            case 0:
                return 1;
            case '*':
                for(;; str++) {
                    if(match(str, ptr+1))
                        return 1;
                    else if(!*str)
                        return 0;
                }
            case '?':
                if(!*str)
                    return 0;
                break;
            case star_replace:
                if(*str != '*')
                    return 0;
                break;
            case question_replace:
                if(*str != '?')
                    return 0;
                break;
            case empty_replace:
                str--;
                break;
            default:
                if(*str != *ptr)
                    return 0;
        }
    }
}

int catch(char *text, char *word, int *text_pos, int *line, int *pos)
{
    int i = *text_pos;
    for(; text[i] == 32 || text[i] == 9 || text[i] == 10 || !text[i]; i++) {
        switch(text[i]) {
            case 32:
            case 9:
                (*pos)++;
                break;
            case 10:
                (*line)++;
                return status_linebreak;
            case 0:
                return status_stop;
        }
    }
    int word_pos = 0;
    for(; text[i] != 32 && text[i] != 9 && text[i] != 10 &&
                                                 text[i]; i++, word_pos++)
        word[word_pos] = text[i];
    word[word_pos] = 0;
    *text_pos = i;
    return status_read;
}

void preprocess(char *ptr)
{
/* Set '*' for better search */
    int len = string_length(ptr);
    ptr[len+1] = 0;
    for(; len > 0; len--)
        ptr[len] = ptr[len-1];
    ptr[0] = '*';
/* Change to special characters to match */
    for(; *ptr; ptr++) {
        if((*ptr == '*' || *ptr == '?') && *(ptr-1) == '\\') {
            switch(*ptr) {
                case '*':
                    *ptr = star_replace;
                    break;
                case '?':
                    *ptr = question_replace;
                    break;
            }
            *(ptr-1) = empty_replace;
        }
    }
}

void show_help()
{
    printf("\
Usage: mft \"-param\" \"[Pattern1]\" \"[Pattern2]\"...\n\n\
Params: -q || --quiet = show matches without line and position\n\
        --help = show help\n\n\
Special characters for patterns:\n\
        '*' = any characters of any length\n\
        '?' = any character, but can not be empty\n\
        '\\' = place it before *, ? and \\ for finding star,\n\
                                           question mark and backslash\n\
Examples:\n\
\"?orem\" \"b*ye\" \"questions\\?\" \"\\*stars\\*\" \"\\\\\"\n\n");
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        fprintf(stderr, "Wrong count of parameters\n");
        return 1;
    }
    int i, k, res, quiet, ptr_count, text_pos, line, pos;
    quiet = 0;
    if(string_compare(argv[1], "--help"))
        show_help();
    else if(string_compare(argv[1], "-q") ||
            string_compare(argv[1], "--quiet"))
        quiet = 1;
    char **ptr = malloc(sizeof(char*)*patterns_buffer_size);
    for(i = 0; i < patterns_buffer_size; i++)
        ptr[i] = malloc(sizeof(char)*word_buffer_size);
    char *word = malloc(sizeof(char)*word_buffer_size);
    char *text = malloc(sizeof(char)*text_buffer_size);
    char *text_cmp = malloc(sizeof(char)*text_buffer_size);
    k = quiet ? 2 : 1;  /* If have param then start loop with 2 */
    for(i = 0; argv[i+k] && i < patterns_buffer_size; i++) {
        string_copy(argv[i+k], ptr[i]);
        preprocess(ptr[i]);
    }
    ptr_count = i;
    text_pos = 0;
    line = pos = 1;
    fgets(text, text_buffer_size, stdin);
    string_copy(text, text_cmp);
    for(;;) {
        res = catch(text, word, &text_pos, &line, &pos);
        switch(res) {
            case status_read:
                for(i = 0; i < ptr_count; i++) {
                    if(match(word, ptr[i])) {
                        if(quiet)
                            printf("%s\n", word);
                        else
                            printf("%d:%d:%s\n", line, pos, word);
                        break;
                    }
                }
                pos += string_length(word);
                break;
            case status_linebreak:
                fgets(text, text_buffer_size, stdin);
                if(string_compare(text, text_cmp)) {
                    res = status_stop;
                    break;
                }
                string_copy(text, text_cmp);
                text_pos = 0;
                pos = 1;
                break;
        }
        if(res == status_stop)
            break;
    }
    for(i = 0; i < patterns_buffer_size; i++)
        free(ptr[i]);
    free(ptr);
    free(word);
    free(text);
    free(text_cmp);
    return 0;
}
