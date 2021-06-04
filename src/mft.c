#include <stdio.h>
#include <stdlib.h>

enum { word_buffer_size = 256, text_buffer_size = 131072,
       star_replacement = 1, question_replacement = 2,
       empty_replacement = 3 };

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
            case star_replacement:
                if(*str != '*')
                    return 0;
                break;
            case question_replacement:
                if(*str != '?')
                    return 0;
                break;
            case empty_replacement:
                str--;
                break;
            default:
                if(*str != *ptr)
                    return 0;
        }
    }
}

int catch(const char *text, int i, char *word)
{
    int tmp_i = 0;
    char *tmp = malloc(sizeof(char)*word_buffer_size);
    for(; text[i] == 32 || text[i] == 9 || text[i] == 10; i++)
        if(!text[i])
            return 0;
    for(; text[i] != 32 && text[i] != 9 &&
                                    text[i] != 10 && text[i]; i++, tmp_i++)
        tmp[tmp_i] = text[i];
    tmp[tmp_i] = 0;
// todo here can add switch for count line and pos
    string_copy(tmp, word);
    free(tmp);
    return i;
}

void preprocess(char *ptr)
{
// Set '*' for better search
    int len = string_length(ptr);
    ptr[len+1] = 0;
    for(; len > 0; len--)
        ptr[len] = ptr[len-1];
    ptr[0] = '*';
// Change to special characters to match
    for(; *ptr; ptr++) {
        if((*ptr == '*' || *ptr == '?') && *(ptr-1) == '\\') {
            switch(*ptr) {
                case '*':
                    *ptr = star_replacement;
                    break;
                case '?':
                    *ptr = question_replacement;
                    break;
            }
            *(ptr-1) = empty_replacement;
        }
    }
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        fprintf(stderr, "Wrong count of parameters\n");
        return 1;
    }
    int i = 0;
    char *ptr = argv[1];
    char *word = malloc(sizeof(char)*word_buffer_size);
    char *text = malloc(sizeof(char)*text_buffer_size);
    preprocess(ptr);
    fgets(text, text_buffer_size, stdin);
    for(;;) {
        if(!text[i] || !(i = catch(text, i, word)))
            break;
        if(match(word, ptr))
            printf("%s\n", word);
    }
    free(word);
    free(text);
    return 0;
}
