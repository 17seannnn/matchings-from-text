#include <stdio.h>
#include <stdlib.h>

enum { patterns_buffer_size = 32, word_buffer_size = 256,
       star_replace = 1, question_replace = 2, empty_replace = 3,
       status_read = 1, status_readln = 2, status_ln = 0, status_readEOF = 3 };

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

int catch(char *word, int *line, int *pos)
{
/* Skip empty spaces and process linebreak and stop statuses */
    int c, i;
    while((c = getchar()) == ' ' || c == 9 || c == '\n' || c == EOF) {
        switch(c) {
            case ' ':
            case 9:     /* Tabulation */
                ++*pos;
                break;
            case '\n':
                return status_ln;
            case EOF:
                return EOF;
        }
    }
/* Catch a word */
    for(i = 0; c != ' ' && c != 9 && c != '\n' && c != EOF; i++) {
        word[i] = c;
        c = getchar();
    }
    word[i] = 0;
    switch(c) {
        case '\n':
            return status_readln;
        case EOF:
            return status_readEOF;
    }
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
    int i, k, res, quiet, pat_count, line, pos;
/* Check a param */
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
/* Copy and preprocess patterns from argv */
    k = quiet ? 2 : 1;  /* If have param then start loop with 2 */
    for(i = 0; argv[i+k] && i < patterns_buffer_size; i++) {
        string_copy(argv[i+k], pat[i]);
        preprocess(pat[i]);
    }
    pat_count = i;
    line = pos = 1;
    for(;;) { /* todo make bools */
        res = catch(word, &line, &pos);
        if(res > 0) {
            for(i = 0; i < pat_count; i++) {
                if(match(word, pat[i])) {
                    if(quiet)
                        printf("%s\n", word);
                    else
                        printf("%d:%d:%s\n", line, pos, word);
                    break;
                }
            }
            pos += string_length(word) + 1; /* + 1 add space pos */
        }
        if(res == status_readln || res == status_ln) {
            line++;
            pos = 1;
        }
        if(res == status_readEOF || res == EOF)
            break;
    }
    for(i = 0; i < patterns_buffer_size; i++)
        free(pat[i]);
    free(pat);
    free(word);
    return 0;
}
