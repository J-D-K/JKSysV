#include <switch.h>
#include <string.h>
#include <malloc.h>

char *newString()
{
    char *ret = malloc(FS_MAX_PATH);
    memset(ret, 0, FS_MAX_PATH);
    return ret;
}
