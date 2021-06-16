#include <stdio.h>
#include <stdlib.h>

enum { patterns_buffer_size = 32, word_buffer_size = 256,
       star_replace = 1, question_replace = 2, empty_replace = 3 };

int str_len(const char *str)
{
    const char *tmp;
    for(tmp = str; *tmp; tmp++)
        {}
    return tmp - str;
}

void str_cpy(const char *src, char *dst)
{
    for(; *src; src++, dst++)
        *dst = *src;
    *dst = 0;
}

int str_cmp(const char *cmp1, const char *cmp2)
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

int catch(char *word, int *is_ln, int *is_eof, int *line, int *pos, FILE *f)
{
/* Skip empty spaces and process linebreak and stop statuses */
    int c, i;
    while((c = fgetc(f)) == ' ' || c == 9 || c == '\n' || c == EOF) {
        switch(c) {
            case ' ':
            case 9:     /* Tabulation */
                (*pos)++;
                break;
            case '\n':
                *is_ln = 1;
                return 0;
            case EOF:
                *is_eof = 1;
                return 0;
        }
    }
/* Catch a word */
    for(i = 0; c != ' ' && c != 9 && c != '\n' && c != EOF; i++) {
        word[i] = c;
        c = fgetc(f);
    }
    word[i] = 0;
    if(c == '\n')
        *is_ln = 1;
    else if(c == EOF)
        *is_eof = 1;
    return 1;
}

void preprocess(char *pat)
{
/* Add '*' to the beginning for better search */
    int len = str_len(pat);
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

void help()
{
    printf("\
Usage: mft -[PARAM] '[Pattern1]' '[Pattern2]'...\n\n\
Params: -q, --quiet = show matches without line and position\n\
        --help      = show help\n\n\
Special characters for patterns:\n\
        '*' = any characters of any length\n\
        '?' = any character, but can not be empty\n\
        '\\' = place it before * and ? for finding star and question mark\n\
Examples:\n\
'?orem' 'b*ye' 'questions\\?' '\\*stars\\*' '\\'\n\n\
If you find bugs - hoggarthlife@gmail.com\n\
Author: https://github.com/17sean\n");
    exit(0);
}

int isparam(const char *str)
{
    if(str_cmp(str, "-q") || str_cmp(str, "--quiet") ||
       str_cmp(str, "-f") || str_cmp(str, "--file") ||
       str_cmp(str, "-p") || str_cmp(str, "--pattern"))
        return 1;
    else
        return 0;
}

void freemem(char **pat, char *word)
{
    for(int i = 0; i < patterns_buffer_size; i++)
        free(pat[i]);
    free(pat); 
    free(word);
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        fprintf(stderr, "Error: wrong count of parameters\n");
        return 1;
    }
    FILE *f;
    int i, k, res, is_ln, is_eof, line, pos;
    int quiet, file, pattern; /* Params */
/* Allocate mem */
    char **pat = malloc(sizeof(char*)*patterns_buffer_size);
    for(i = 0; i < patterns_buffer_size; i++)
        pat[i] = malloc(sizeof(char)*word_buffer_size);
    char *word = malloc(sizeof(char)*word_buffer_size);
/* Check params */
    quiet = 0;
    file = 0;
    pattern = 0;
    for(i = 0; argv[i]; i++) {
        if(str_cmp(argv[i], "--help"))
            help();
        else if(str_cmp(argv[i], "-q") || str_cmp(argv[i], "--quiet"))
            quiet = 1;
        else if(str_cmp(argv[i], "-f") || str_cmp(argv[i], "--file"))
            file = 1;
        else if(str_cmp(argv[i], "-p") || str_cmp(argv[i], "--pattern"))
            pattern = 1;
    }
    /*
     * If we have -f param but dont have -p
     * then mft can not understand where
     * exactly the patterns are.
     */
    if(file && !pattern) {
        fprintf(stderr, "Error: add -p param before patterns\n");
        freemem(pat, word);
        return 2;
    }
    /* TODO check for files
     * if(files) then we are looking
     *     for them from -f param
     * else
     *     f = stdin;
     */ 
    /*
     * If we have "-p" param
     * then we are looking for it.
     * Else skip all params
     */
    if(pattern) {
        for(i = 1; !str_cmp(argv[i], "-p") &&
                   !str_cmp(argv[i], "--pattern"); i++)
            {}
        i++;
    } else {
        for(i = 1; argv[i] && isparam(argv[i]); i++)
            {}
    }
    pattern = 0; /* start counting */
    /*TODO пока argv[i] -> делаем всю тему и k++ */
    pattern = k;
    if(!pattern) {
        fprintf(stderr, "Error: no patterns given\n");
        freemem(pat, word);
        return 3;
    }
/* Copy, preprocess and count patterns from argv */
/*
    for(k = 0; argv[i] && k < patterns_buffer_size; i++) {
        str_cpy(argv[i+k], pat[i]);
        preprocess(pat[i]);
    }
    patterns = i;
*/
    line = pos = 1;
    is_ln = is_eof = 0;
    f = stdin; /* TODO temp */
    while(!is_eof) {
        res = catch(word, &is_ln, &is_eof, &line, &pos, f);
        /*
         * If res == 1 then we
         * can compare a word
         */
        if(res) {
            for(i = 0; i < pattern; i++) {
                if(match(word, pat[i])) {
                    if(quiet)
                        printf("%s\n", word);
                    else
                        printf("%d:%d:%s\n", line, pos, word);
                    break;
                }
            }
            pos += str_len(word) + 1; /* + 1 for space and tab */
        }
        if(is_ln) {
            line++;
            pos = 1;
            is_ln = 0;
        }
    }
    freemem(pat, word);
    return 0;
}
