#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

// Функция сравнения для qsort, использующая strcoll
int compare_strings(const void *a, const void *b)
{
    return strcoll(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");

    int opt;
    int l_flag =   0, d_flag =   0, f_flag =   0, s_flag =   0;
    char *dir = ".";

    while ((opt = getopt(argc, argv, "ldfs")) != -1)
    {
        switch (opt) {
            case 'l':
                l_flag =   1;
                break;
            case 'd':
                d_flag =   1;
                break;
            case 'f':
                f_flag =   1;
                break;
            case 's':
                s_flag =   1;
                break;
            default:
                fprintf(stderr, "Неизвестная опция: %c\n", optopt);
                return   1;
        }
    }
    
    if (optind < argc)
    {
        dir = argv[optind];
    }

    DIR *d = opendir(dir); 
    if (d) {
        struct dirent *entry;
        char **files = NULL; // Указатель на массив для хранения имен файлов
        size_t files_count =   0; // Счетчик файлов

        while ((entry = readdir(d)) != NULL) {
            struct stat info;
            stat(entry->d_name, &info);

            if ((l_flag && S_ISLNK(info.st_mode)) ||
                (d_flag && S_ISDIR(info.st_mode)) ||
                (f_flag && S_ISREG(info.st_mode)) ||
                (!l_flag && !d_flag && !f_flag))
            {
                files = realloc(files, sizeof(char *) * (files_count+1));
                if (files == NULL)
                {
                    perror("Ошибка выделения памяти");
                    return  1;
                }
                files[files_count] = strdup(entry->d_name);
                if (files[files_count] == NULL) {
                    perror("Ошибка копирования строки");
                    return  1;
                }
                files_count++;
            }
        }
        closedir(d);

        if (s_flag)
        {
            qsort(files, files_count, sizeof(char *), compare_strings);
        }

        for (size_t i =   0; i < files_count; i++)
        {
            printf("%s\n", files[i]);
            free(files[i]);
        }
        free(files);
    } else
    {
        perror("Ошибка при открытии каталога.");
        return   1;
    }
    return   0;
}
