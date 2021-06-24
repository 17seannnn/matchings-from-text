#include <stdio.h>
#include <stdlib.h>

enum {
       mft_version_main     = 1,
       mft_version_other    = 21,

       files_buffer_size    = 64,
       patterns_buffer_size = 64,
       word_buffer_size     = 1024,

       help_full_param      = 1,
       help_short_param     = 2,
       version_full_param   = 3,
       version_short_param  = 4,
       qni_param            = 5,
       quiet_param          = 6,
       ignore_case_param    = 7,
       file_param           = 8,
       pattern_param        = 9,

       star_replace         = 1,
       question_replace     = 2,
       empty_replace        = 3
     };

void help_full()
{
    printf("\
Usage: mft -[PARAMs] '[Pattern1]' '[Pattern2]'...\n\n\
Params\n\
        -h, --help        = show help.\n\
        -v, --version     = show version.\n\
        -q, --quiet       = show matchings without file name, line and pos.\n\
        -i, --ignore-case = ignore lowercase and uppercase.\n\
        -qi, -iq          = --quiet and --ignore-case in one param.\n\
        -f, --file        = declare files to search.\n\
        -p, --pattern     = declare patterns to match.\n\n\
Patterns\n\
    Special characters for patterns\n\
        '*' = any character string of any length.\n\
        '?' = any character, but can not be empty.\n\
        '\\' = place it before * and ? for finding star and question mark.\n\
    Examples\n\
        '?orem' 'b*ye' 'questions\\?' '\\*stars\\*' '\\'\n\n\
If you find bugs:  17seannnn@gmail.com\n\
mft home page:     <https://github.com/17sean/matchings-from-text>\n");
}

void help_short()
{
    printf("\
Usage: mft -[PARAMs] '[Pattern1]' '[Pattern2]'...\n\
For more information use '--help'.\n");
}

void version_full()
{
    printf("\
mft (matchings-from-text) v%d.%d\n\
Copyright (c) 2021 17sean\n\
License MIT: <https://opensource.org/licenses/MIT>\n\n\
Written by 17sean.\n\
Github: <https://github.com/17sean>\n",
mft_version_main, mft_version_other);
}

void version_short()
{
    printf("\
mft (matchings-from-text) v%d.%d\n\
For more information use '--version'.\n",
mft_version_main, mft_version_other);
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

int which_param(const char *str)
{
    if(str_cmp(str, "--help"))
        return help_full_param;
    else if(str_cmp(str, "-h"))
        return help_short_param;
    else if(str_cmp(str, "--version"))
        return version_full_param;
    else if(str_cmp(str, "-v"))
        return version_short_param;
    else if(str_cmp(str, "-qi") || str_cmp(str, "-iq"))
        return qni_param;
    else if(str_cmp(str, "-q")  || str_cmp(str, "--quiet"))
        return quiet_param;
    else if(str_cmp(str, "-i")  || str_cmp(str, "--ignore-case"))
        return ignore_case_param;
    else if(str_cmp(str, "-f")  || str_cmp(str, "--file"))
        return file_param;
    else if(str_cmp(str, "-p")  || str_cmp(str, "--pattern"))
        return pattern_param;
    else
        return 0;
}

int is_letter(char c)
{
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return 1;
    else
        return 0;
}

void preprocess(char *pat)
{
/* Add '*' to the beginning for better search */
    int i = str_len(pat) + 1;  /* + 1 to count '\0' */
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

int
init_param(int *quiet, int *ignore_case, int *file, int *pattern, char **argv)
{
    *quiet     = 0;
    *ignore_case = 0;
    *file      = 0;
    *pattern   = 0;
    for(int i = 1; argv[i]; i++) {
        switch(which_param(argv[i])) {
            case help_full_param:
                help_full();
                *file = 0;
                /* 
                * 111 mean func completed
                * successfully, goto quit
                */
                return 111; 
            case help_short_param:
                help_short();
                *file = 0;
                return 111; 
            case version_full_param:
                version_full();
                *file = 0;
                return 111; 
            case version_short_param:
                version_short();
                *file = 0;
                return 111; 
            case qni_param:
                *quiet = 1;
                *ignore_case = 1;
                break;
            case quiet_param:       *quiet = 1;         break;
            case ignore_case_param: *ignore_case = 1;   break;
            case file_param:        *file = 1;          break;
            case pattern_param:     *pattern = 1;       break;
        }
    }
    /*
     * If we have -f param but dont have -p
     * then mft cannot understand where
     * exactly the patterns are.
     */
    if(*file && !(*pattern)) {
        fprintf(stderr, "Error: Add -p param before patterns\n");
        *file = 0;
        return 0;
    }
    return 1;
}

int init_file(int *file, FILE **f, char **fname, char **argv)
{
    /*
     * If we have "-f" param
     * then we are looking for it
     * and open streams.
     */
    int i, k;
    if(*file) {
        for(i = 0; which_param(argv[i]) != file_param; i++)
            {}
        i++;
        for(k = 0; argv[i] && !which_param(argv[i]) &&
                                            k < files_buffer_size; i++, k++) {
            f[k] = fopen(argv[i], "r");
            if(!f[k]) {
                perror(argv[i]);
                *file = k;
                return 0;
            }
            str_cpy(argv[i], fname[k], word_buffer_size);
        }
        *file = k;    /* k is number of files */
    }
    /*
     * If do not have -f param or
     * nothing after param  
     */
    if(!(*file)) {
        f[0] = stdin;
        *file = 1;
    }
    return 1; 
}

int init_pattern(int *pattern, char **pat, char **argv)
{
    /*
     * If we have "-p" param
     * then we are looking for it.
     * Else skip all params
     */
    int i, k;
    if(*pattern) {
        for(i = 1; which_param(argv[i]) != pattern_param; i++)
            {}
        i++;
    } else {
        for(i = 1; argv[i] && which_param(argv[i]); i++)
            {}
    }
    for(k = 0; argv[i] && !which_param(argv[i]) &&
                                        k < patterns_buffer_size; i++, k++) {
        str_cpy(argv[i], pat[k], word_buffer_size);
        preprocess(pat[k]);
    }
    *pattern = k;    /* k is number of patterns */
    if(!(*pattern)) {
        fprintf(stderr, "Error: No patterns given\n");
        return 0;
    }
    return 1;
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

int match(const char *str, const char *pat, int ignore_case)
{
    for(;; str++, pat++) {
        switch(*pat) {
            case 0:
                return 1;
            case '*':
                for(;; str++) {
                    if(match(str, pat+1, ignore_case))
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
                    if(ignore_case && is_letter(*str) && is_letter(*pat)) {
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
    int quiet, ignore_case, file, pattern; /* Params */
/* Allocate mem */
    FILE **f = malloc(sizeof(FILE*)*files_buffer_size);
    char **fname = malloc(sizeof(char*)*files_buffer_size);
    for(i = 0; i < files_buffer_size; i++)
        fname[i] = malloc(sizeof(char)*word_buffer_size);
    char **pat = malloc(sizeof(char*)*patterns_buffer_size);
    for(i = 0; i < patterns_buffer_size; i++)
        pat[i] = malloc(sizeof(char)*word_buffer_size);
    char *word = malloc(sizeof(char)*word_buffer_size);
/* Init */
    /* Param */
    res = init_param(&quiet, &ignore_case, &file, &pattern, argv); 
    /* 
     * init_param() return 111 
     * when find help param
     */
    if(res == 111) {
        goto quit;
    } else if(!res) {
        exit_status = 1;
        goto quit;
    }
    /* File */
    res = init_file(&file, f, fname, argv);
    if(!res) {
        exit_status = 2;
        goto quit;
    }
    /* Pattern */
    res = init_pattern(&pattern, pat, argv);
    if(!res) {
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
                    if(match(word, pat[k], ignore_case)) {
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
