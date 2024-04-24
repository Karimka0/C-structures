#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

int main() {
    int stack_capacity = 1;
    int stack_size = 0;
    char** stack = static_cast<char**>(malloc(stack_capacity * sizeof(char*)));
    char command[6];
    while (scanf("%s", command) != EOF) {
        if (stack_size >= stack_capacity - 1) {
            stack_capacity *= 2;
            stack =
                    static_cast<char**>(realloc(stack, stack_capacity * sizeof(char*)));
        }
        if (strcmp(command, "push") == 0) {
            int str_capacity = 2;
            int str_size = 0;
            char* str = static_cast<char*>(malloc(str_capacity * sizeof(char)));
            getchar();
            char c;
            while (!std::isspace(c = getchar()) && c != EOF) {
                if (str_size >= str_capacity - 1) {
                    str_capacity *= 2;
                    str = static_cast<char*>(realloc(str, str_capacity * sizeof(char)));
                }
                str[str_size] = c;
                ++str_size;
            }
            str[str_size] = '\0';
            str = static_cast<char*>(realloc(str, str_size + 1));
            stack[stack_size] = str;
            ++stack_size;
            printf("ok\n");
            continue;
        }
        if (strcmp(command, "pop") == 0) {
            if (stack_size > 0) {
                printf("%s\n", stack[stack_size - 1]);
                free(stack[stack_size - 1]);
                --stack_size;
                if (stack_size <= stack_capacity / 4 && stack_size != 0) {
                    stack_capacity /= 2;
                    stack = static_cast<char**>(
                            realloc(stack, stack_capacity * sizeof(char*)));
                }
            } else {
                printf("error\n");
            }
            continue;
        }
        if (strcmp(command, "back") == 0) {
            if (stack_size > 0) {
                printf("%s\n", stack[stack_size - 1]);
            } else {
                printf("error\n");
            }
            continue;
        }
        if (strcmp(command, "size") == 0) {
            printf("%d\n", stack_size);
            continue;
        }
        if (strcmp(command, "clear") == 0) {
            for (int i = 0; i < stack_size; ++i) {
                free(stack[i]);
            }
            stack_size = 0;
            free(stack);
            stack = static_cast<char**>(malloc(stack_capacity * sizeof(char*)));
            printf("ok\n");
            continue;
        }
        if (strcmp(command, "exit") == 0) {
            printf("bye\n");
            break;
        }
    }
    for (int i = 0; i < stack_size; ++i) {
        free(stack[i]);
    }
    free(stack);
    return 0;
}