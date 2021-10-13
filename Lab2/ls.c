#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_help(int argc, char* argv[])
{
    printf("Usage: %s directory\n"
           "Prints files in directory"
           "\t--help - show this message", argv[0]);
}

void format_mode(mode_t mode, char* mode_str)
{
    if (S_ISDIR(mode))
        mode_str[0] = 'd';

    if (S_ISLNK(mode))
        mode_str[0] = 'l';

    if (mode & S_IRUSR)
        mode_str[1] = 'r';

    if (mode & S_IWUSR)
        mode_str[2] = 'w';

    if (mode & S_IXUSR)
        mode_str[3] = 'x';

    if (mode & S_IRGRP)
        mode_str[4] = 'r';

    if (mode & S_IWGRP)
        mode_str[5] = 'w';

    if (mode & S_IXGRP)
        mode_str[6] = 'x';

    if (mode & S_IROTH)
        mode_str[7] = 'r';

    if (mode & S_IWOTH)
        mode_str[8] = 'w';

    if (mode & S_IXOTH)
        mode_str[9] = 'x';
}

void print_dir_entry(const char* dirname, struct dirent* ent)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dirname, ent->d_name);

    struct stat st;
    if (stat(path, &st) != 0)
        return;

    char mode[11] = "----------";
    format_mode(st.st_mode, mode);

    unsigned links = st.st_nlink;

    struct passwd* pwd = getpwuid(st.st_uid);
    struct group* grp = getgrgid(st.st_uid);

    const char* owner = pwd->pw_name;
    const char* group = grp->gr_name;

    size_t size = st.st_size;

    char* date = ctime(&st.st_ctim.tv_sec);
    date[strlen(date) - 1] = '\0';

    const char* name = ent->d_name;

    printf("%s %3u %-7s %-7s %10lu %12s %s\n",
           mode, links, owner, group, size, date, name);
}

int main(int argc, char* argv[])
{
    if (argc != 2 || strcmp("--help", argv[1]) == 0)
    {
        print_help(argc, argv);
        return 1;
    }

    const char* path = argv[1];
    struct stat path_stat;

    if (stat(path, &path_stat) != 0)
        return -1;

    if (!S_ISDIR(path_stat.st_mode))
        return -1;

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return 0;

    while ((entry = readdir(dir)) != NULL)
    {
        print_dir_entry(path, entry);
    }
    closedir(dir);

}
