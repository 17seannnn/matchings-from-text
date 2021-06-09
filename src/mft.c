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

int match(const char *str, const char *pat)
{
    for(;; str++, pat++) {
        switch(*pat) {
            case 0:
                return 1;
            case '*':
                for(;; str++) {
                    if(match(str, pat+1))
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
                if(*str != *pat)
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

void preprocess(char *pat)
{
/* Set '*' for better search */
    int len = string_length(pat);
    pat[len+1] = 0;
    for(; len > 0; len--)
        pat[len] = pat[len-1];
    pat[0] = '*';
/* Change to special characters to match */
    for(; *pat; pat++) {
        if((*pat == '*' || *pat == '?') && *(pat-1) == '\\') {
            switch(*pat) {
                case '*':
                    *pat = star_replace;
                    break;
                case '?':
                    *pat = question_replace;
                    break;
            }
            *(pat-1) = empty_replace;
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
    int i, k, res, quiet, pat_count, text_pos, line, pos;
    quiet = 0;
    if(string_compare(argv[1], "--help"))
        show_help();
    else if(string_compare(argv[1], "-q") ||
            string_compare(argv[1], "--quiet"))
        quiet = 1;
    char **pat = malloc(sizeof(char*)*patterns_buffer_size);
    for(i = 0; i < patterns_buffer_size; i++)
        pat[i] = malloc(sizeof(char)*word_buffer_size);
    char *word = malloc(sizeof(char)*word_buffer_size);
    char *text = malloc(sizeof(char)*text_buffer_size);
    char *text_cmp = malloc(sizeof(char)*text_buffer_size);
    k = quiet ? 2 : 1;  /* If have param then start loop with 2 */
    for(i = 0; argv[i+k] && i < patterns_buffer_size; i++) {
        string_copy(argv[i+k], pat[i]);
        preprocess(pat[i]);
    }
    pat_count = i;
    text_pos = 0;
    line = pos = 1;
    fgets(text, text_buffer_size, stdin);
    string_copy(text, text_cmp);
    for(;;) {
        res = catch(text, word, &text_pos, &line, &pos);
        switch(res) {
            case status_read:
                for(i = 0; i < pat_count; i++) {
                    if(match(word, pat[i])) {
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
        free(pat[i]);
    free(pat);
    free(word);
    free(text);
    free(text_cmp);
    return 0;
}
