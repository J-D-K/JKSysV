#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <switch.h>
#include <sys/stat.h>

#include "fs.h"
#include "misc.h"

#define INNER_HEAP 0x80000

#ifdef __cplusplus
extern "C"
{
#endif
u32 __nx_applet_type = AppletType_None;
u32 __nx_fs_num_sessions = 1;

void __libnx_initheap(void)
{
    static u8 innerHeap[INNER_HEAP];
    extern void *fake_heap_start, *fake_heap_end;
    fake_heap_start = innerHeap;
    fake_heap_end   = innerHeap + INNER_HEAP;
}

void __appInit(void)
{
    if(R_FAILED(smInitialize()))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    SetSysFirmwareVersion fw;
    if(R_SUCCEEDED(setsysInitialize()) && R_SUCCEEDED(setsysGetFirmwareVersion(&fw)))
    {
        hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }

    if(R_FAILED(fsInitialize()))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    pmdmntInitialize();

    fsdevMountSdmc();
}

void __appExit(void)
{
    fsdevUnmountAll();
    pmdmntExit();
    fsExit();
}
#ifdef __cplusplus
}
#endif

const char *tidPath = "sdmc:/config/JKSV/JKSysV.txt";
uint64_t *tids;
uint64_t tidRunning;
unsigned int tidCount = 0;
uint8_t state = 0;
enum
{
    STATE_CHECK_RUNNING,
    STATE_RUNNING,
};

void debugOut(const char *path)
{
    FILE *deb = fopen(path, "w");
    fclose(deb);
}

void tidsInit()
{
    tids = (uint64_t *)malloc(sizeof(uint64_t));
}

void tidsAdd(uint64_t _newTid)
{
    tids = (uint64_t *)realloc(tids, sizeof(uint64_t) * ++tidCount);
    tids[tidCount - 1] = _newTid;
}

void tidsExit()
{
    free(tids);
}

void tidsLoad()
{
    FILE *in = fopen(tidPath, "r");
    if(in)
    {
        char line[32];
        while(fgets(line, 32, in))
        {
            uint64_t tidIn = strtoul(line, NULL, 16);
            tidsAdd(tidIn);
        }
        fclose(in);
    }
}

void checkTIDSRunning()
{
    for(unsigned i = 0; i < tidCount; i++)
    {
        uint64_t pid = 0;
        if(R_SUCCEEDED(pmdmntGetProcessId(&pid, tids[i])))
        {
            tidRunning = tids[i];
            state = STATE_RUNNING;
            break;
        }
    }
}

void checkStillRunning()
{
    uint64_t pid = 0;
    if(R_FAILED(pmdmntGetProcessId(&pid, tidRunning)))
    {
        svcSleepThread(5e+9);//Give some shut down time.
        if(mountSave(tidRunning))
        {
            char *dst = newString();
            sprintf(dst, "sdmc:/JKSV/%016lX", tidRunning);
            mkdir(dst, 0777);
            strcat(dst, "/");

            copyDirToDir("sv:/", dst);
            fsdevUnmountDevice("sv");
            free(dst);
        }
        state = STATE_CHECK_RUNNING;
    }
}

int main(int argc, const char *argv[])
{
    tidsInit();
    tidsLoad();

    while(true)
    {
        switch(state)
        {
            case STATE_CHECK_RUNNING:
                checkTIDSRunning();
                break;

            case STATE_RUNNING:
                checkStillRunning();
                break;
        }
        svcSleepThread(1e+9);
    }

    tidsExit();
}
