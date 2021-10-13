#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "vec.h"

const unsigned MAX_PATH_SIZE = 1024;

void print_help(int argc, char* argv[])
{
    printf("Usage: %s files...\n"
           "Prints hardlinks in directories\n"
           "\t--help - show this message", argv[0]);
}

int recurve(const char* path, int (*func)(const char* path))
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return 0;

    while ((entry = readdir(dir)) != NULL)
    {
        char name[4096];
        if (strcmp(path, "/") == 0)
            snprintf(name, sizeof(name), "/%s", entry->d_name);
        else
            snprintf(name, sizeof(name), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
                continue;

            if (recurve(name, func) == -1)
                return -1;
        } else {
            if (func(name) == -1)
                return -1;
        }
    }
    closedir(dir);
    return 0;
}

const char* to_check;
vec inodes;
vec files;
int cmpfunc (const void * a, const void * b)
{
    if(*(ino_t*)a < *(ino_t*)b)
        return -1;
    if(*(ino_t*)a > *(ino_t*)b > 0)
        return 1;
   return 0;
}

int print_if_same(const char* path)
{
    struct stat buf;
    if (lstat(path, &buf) == 0)
    {
        if (buf.st_nlink > 1)
        {

            if (vec_binary(&inodes, &buf.st_ino, cmpfunc))
            {
                vec_push_back_uninitialized(&files);
                char* str = vec_get(&files, files.size - 1);
                snprintf(str, MAX_PATH_SIZE,
                         "%lu : %s", buf.st_ino, path);
            }
        }
    }

    return 0;
}

int save_inodes(const char* path)
{
    struct stat buf;
    if (lstat(path, &buf) == 0)
    {
        if (buf.st_nlink > 1)
        {
            vec_push_back(&inodes, &buf.st_ino);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2 || strcmp("--help", argv[1]) == 0)
    {
        print_help(argc, argv);
        return 1;
    }

    vec_init(&inodes, sizeof(ino_t));
    vec_init(&files, sizeof(char[MAX_PATH_SIZE]));

    const char* root_path = argv[1];
    //Recurve over given path and save all inodes with multiple hardlinks
    recurve(root_path, save_inodes);

    vec_qsort(&inodes, cmpfunc);

    vec_unique(&inodes, cmpfunc);
    printf("Found %lu hardlinked inodes\n", inodes.size);

    //Recurve over whole filesystem and find inodes
    recurve("/", print_if_same);

    vec_qsort(&files, strcmp);
    vec_print(&files, "%s\n");
    vec_destroy(&inodes);
    return 0;
}
