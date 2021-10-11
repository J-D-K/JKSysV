#include <switch.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "fs.h"
#include "misc.h"

bool mountSave(uint64_t tid)
{
    accountInitialize(AccountServiceType_System);
    AccountUid uid;
    accountGetLastOpenedUser(&uid);
    accountExit();

    FsFileSystem sv;
    if(R_SUCCEEDED(fsOpen_SaveData(&sv, tid, uid)))
    {
        fsdevMountDevice("sv", sv);
        return true;
    }
    return false;
}

bool isDir(const char *path)
{
    struct stat s;
    return stat(path, &s) == 0 && S_ISDIR(s.st_mode);
}

void copyFile(const char *src, const char *dst)
{
    FILE *fsrc = fopen(src, "rb");
    FILE *fdst = fopen(dst, "wb");

    if(!fsrc || !fdst)
    {
        fclose(fsrc);
        fclose(fdst);
        return;
    }

    u8 *buff = malloc(0x60000);
    if(!buff)
        return;

    size_t readIn = 0;
    while((readIn = fread(buff, 1, 0x40000, fsrc)) > 0)
        fwrite(buff, 1, readIn, fdst);

    fclose(fsrc);
    fclose(fdst);
    free(buff);
}

void copyDirToDir(const char *src, const char *dst)
{
    DIR *d = opendir(src);
    struct dirent *ent;
    while((ent = readdir(d)))
    {
        char *fullSrc = newString();
        char *fullDst = newString();
        sprintf(fullSrc, "%s%s", src, ent->d_name);
        sprintf(fullDst, "%s%s", dst, ent->d_name);
        if(isDir(fullSrc))
        {
            mkdir(fullDst, 0777);
            strcat(fullSrc, "/");
            strcat(fullDst, "/");
            copyDirToDir(fullSrc, fullDst);
        }
        else
            copyFile(fullSrc, fullDst);

        free(fullSrc);
        free(fullDst);
    }
    closedir(d);
}
