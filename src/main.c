#include <stdio.h>
#include <stdlib.h>

enum { files_buffer_size    = 64,
       patterns_buffer_size = 64,
       word_buffer_size     = 1024,

       help_param           = 1,
       qnc_param            = 2,
       quiet_param          = 3,
       any_cases_param      = 4,
       file_param           = 5,
       pattern_param        = 6,

       star_replace         = 1,
       question_replace     = 2,
       empty_replace        = 3 };

void help()
{
    printf("\
Usage: mft -[PARAM] '[Pattern1]' '[Pattern2]'...\n\n\
Params: -q, --quiet = show matches without line and position\n\
        --help      = show help\n\n\
Special characters for patterns:\n\
        '*' = any characters of any length\n\
        '?' = any character, but cannot be empty\n\
        '\\' = place it before * and ? for finding star and question mark\n\
Examples:\n\
'?orem' 'b*ye' 'questions\\?' '\\*stars\\*' '\\'\n\n\
If you find bugs - hoggarthlife@gmail.com\n\
Author:            https://github.com/17sean\n");
}

int str_len(const char *str)
{
    const char *tmp;
    for(tmp = str; *tmp; tmp++)
        {}
    return tmp - str;
}

void str_cpy(const char *src, char *dst, int size)
{
    char *tmp = dst;
    for(; *src && (tmp - dst < size - 1); src++, tmp++)
        *tmp = *src;
    *tmp = 0;
}

int str_cmp(const char *cmp1, const char *cmp2)
{
    for(; *cmp1 || *cmp2; cmp1++, cmp2++) {
        if(*cmp1 != *cmp2)
            return 0;
    }
    return 1;
}

int whichparam(const char *str)
{
    if(str_cmp(str, "--help"))
        return help_param;
    else if(str_cmp(str, "-qc") || str_cmp(str, "-cq"))
        return qnc_param;
    else if(str_cmp(str, "-q")  || str_cmp(str, "--quiet"))
        return quiet_param;
    else if(str_cmp(str, "-c")  || str_cmp(str, "--any-cases"))
        return any_cases_param;
    else if(str_cmp(str, "-f")  || str_cmp(str, "--file"))
        return file_param;
    else if(str_cmp(str, "-p")  || str_cmp(str, "--pattern"))
        return pattern_param;
    else
        return 0;
}

int isparam(const char *str)
{
    /*
     * If whichparam() return
     * value > 0 then it is param
     */
    return whichparam(str);
}

int isletter(char c)
{
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return 1;
    else
        return 0;
}

void preprocess(char *pat)
{
/* Add '*' to the beginning for better search */
    int i = str_len(pat) + 1;
    if(i >= word_buffer_size) {
        i = word_buffer_size - 1;
        pat[i] = 0;
        i--;
    }
    for(; i > 0; i--)
        pat[i] = pat[i-1];
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
    for(i = 0; i < word_buffer_size - 1 && c != ' ' && c != 9 &&
                                                 c != '\n' && c != EOF; i++) {
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

int match(const char *str, const char *pat, int any_cases)
{
    for(;; str++, pat++) {
        switch(*pat) {
            case 0:
                return 1;
            case '*':
                for(;; str++) {
                    if(match(str, pat+1, any_cases))
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
                if(*str != *pat) {
                    if(any_cases && isletter(*str) && isletter(*pat)) {
                        if(*str != *pat - 32 && *str != *pat + 32)
                            return 0;
                    } else
                        return 0;
                }
        }
    }
}

int main(int argc, char **argv)
{
    int exit_status = 0;
    int i, k; /* For loops */
    int res, is_ln, is_eof, line, pos; /* Main vars */
    int quiet, any_cases, file, pattern; /* Params */
/* Allocate mem */
    FILE **f = malloc(sizeof(FILE*)*files_buffer_size);
    char **fname = malloc(sizeof(char*)*files_buffer_size);
    for(i = 0; i < files_buffer_size; i++)
        fname[i] = malloc(sizeof(char)*word_buffer_size);
    char **pat = malloc(sizeof(char*)*patterns_buffer_size);
    for(i = 0; i < patterns_buffer_size; i++)
        pat[i] = malloc(sizeof(char)*word_buffer_size);
    char *word = malloc(sizeof(char)*word_buffer_size);
/* Check params */
    quiet     = 0;
    any_cases = 0;
    file      = 0;
    pattern   = 0;
    for(i = 1; argv[i]; i++) {
        switch(whichparam(argv[i])) {
            case help_param:
                help();
                file = 0;
                goto quit;
            case qnc_param:
                quiet = 1;
                any_cases = 1;
                break;
            case quiet_param:     quiet = 1;     break;
            case any_cases_param: any_cases = 1; break;
            case file_param:      file = 1;      break;
            case pattern_param:   pattern = 1;   break;
        }
    }
    /*
     * If we have -f param but dont have -p
     * then mft cannot understand where
     * exactly the patterns are.
     */
    if(file && !pattern) {
        fprintf(stderr, "Error: Add -p param before patterns\n");
        file = 0;
        exit_status = 1;
        goto quit;
    }
    /*
     * If we have "-f" param
     * then we are looking for it
     * and open streams.
     */
    if(file) {
        for(i = 0; !str_cmp(argv[i], "-f") && !str_cmp(argv[i], "--file"); i++)
            {}
        i++;
        for(k = 0; argv[i] && !isparam(argv[i]) &&
                                            k < files_buffer_size; i++, k++) {
            f[k] = fopen(argv[i], "r");
            if(!f[k]) {
                perror(argv[i]);
                file = k;
                exit_status = 2;
                goto quit;
            }
            str_cpy(argv[i], fname[k], word_buffer_size);
        }
        file = k;    /* k is number of files */
    }
    /*
     * If don`t have -f param or
     * nothing after param  
     */
    if(!file) {
        f[0] = stdin;
        file = 1;
    }
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
    for(k = 0; argv[i] && !isparam(argv[i]) &&
                                        k < patterns_buffer_size; i++, k++) {
        str_cpy(argv[i], pat[k], word_buffer_size);
        preprocess(pat[k]);
    }
    pattern = k;    /* k is number of patterns */
    if(!pattern) {
        fprintf(stderr, "Error: No patterns given\n");
        exit_status = 3;
        goto quit;
    }
/* Main loop */
    for(i = 0; i < file; i++) {
        line = 1;
        pos = 1;
        is_ln = 0;
        is_eof = 0;
        if(!quiet && f[i] != stdin)
            printf("/* %s */\n", fname[i]);
        while(!is_eof) {
            res = catch(word, &is_ln, &is_eof, &line, &pos, f[i]);
            /*
             * If res == 1 then we
             * can compare a word
             */
            if(res) {
                for(k = 0; k < pattern; k++) {
                    if(match(word, pat[k], any_cases)) {
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
    }
quit:
/* Close all streams */
    for(i = 0; i < file; i++) {
        if(fclose(f[i]))
            perror("close stream");
    }
/* Free memory */
    free(f);
    for(i = 0; i < files_buffer_size; i++)
        free(fname[i]);
    free(fname); 
    for(i = 0; i < patterns_buffer_size; i++)
        free(pat[i]);
    free(pat); 
    free(word);
    return exit_status;
}
