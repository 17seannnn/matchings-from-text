#include <stdio.h>
#include <stdlib.h>

enum { word_buffer_size = 256, text_buffer_size = 131072 };

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

int match(const char *str, const char *ptr)
{
    for(;; str++, ptr++) {
        switch(*ptr) {
            case 0:
                return 1;
            case '*':
                for(;; str++)
                    if(match(str, ptr+1))
                        return 1;
                    else if(!*str)
                        return 0;
                break;
            case '?':
                if(!*str)
                    return 0;
                break;
            default:
                if(*str != *ptr)
                    return 0;
        }
    }
}

int catch_word(const char *txt, int i, char *word)
{
    int tmp_i = 0;
    char *tmp = malloc(sizeof(char)*word_buffer_size);
    for(; txt[i] == 32 || txt[i] == 9 || txt[i] == 10; i++)
        if(!txt[i])
            return 0;
    for(; txt[i] != 32 && txt[i] != 9 && txt[i] != 10 && txt[i]; i++, tmp_i++)
        tmp[tmp_i] = txt[i];
    tmp[tmp_i] = 0;
// here can add switch for count line and pos
    string_copy(tmp, word);
    free(tmp);
    return i;
}

void preprocessing_ptr(char *ptr)
{
    int len = string_length(ptr);
    ptr[len+1] = 0;
    for(; len > 0; len--)
        ptr[len] = ptr[len-1];
    ptr[0] = '*';
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Wrong count of parameters\n");
        return 1;
    }
    int i = 0;
    char *ptr = argv[1];
    char *word = malloc(sizeof(char)*word_buffer_size);
    char *txt = malloc(sizeof(char)*text_buffer_size);
    preprocessing_ptr(ptr);
    fgets(txt, text_buffer_size, stdin);
    for(;;) {
        i = catch_word(txt, i, word);
        if(match(word, ptr))
            printf("%s\n", word);
        if(!i || !txt[i])
            break;
    }
    free(word);
    free(txt);
    return 0;
}
