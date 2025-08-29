#include "JKSysV.hpp"
#include "json.hpp"

#include <switch.h>

namespace
{
    /// @brief Heap size to use.
    constexpr int HEAP_SIZE = 0x100000;
}

template <typename... Args>
static void initialize_service(Result (*function)(Args...), Args &&...args)
{
    const bool success = R_SUCCEEDED((*function)(args...));
    if (!success) { diagAbortWithResult(success); }
}

static inline void initialize_libnx_firm_version()
{
    const bool setSysInit = R_SUCCEEDED(setsysInitialize());
    if (!setSysInit) { return; }

    SetSysFirmwareVersion firmVersion{};
    const bool getVersion = R_SUCCEEDED(setsysGetFirmwareVersion(&firmVersion));
    if (!getVersion)
    {
        setsysExit();
        return;
    }

    const uint32_t version = MAKEHOSVERSION(firmVersion.major, firmVersion.minor, firmVersion.micro);
    hosversionSet(version);
    setsysExit();
}

static inline void initialize_libnx_time()
{
    const bool timeInit = R_SUCCEEDED(timeInitialize());
    if (!timeInit) { return; }

    extern void __libnx_init_time();
    timeExit();
}

extern "C"
{
    uint32_t __nx_applet_type     = AppletType_None;
    uint32_t __nx_fs_num_sessions = 1;

    void __libnx_initheap()
    {
        static constinit std::array<unsigned char, HEAP_SIZE> innerHeap{};
        extern void *fake_heap_start, *fake_heap_end;

        fake_heap_start = innerHeap.data();
        fake_heap_end   = innerHeap.data() + HEAP_SIZE;
    }

    void __appInit()
    {
        initialize_service(smInitialize);
        initialize_libnx_firm_version();
        initialize_service(accountInitialize, AccountServiceType_System);
        initialize_libnx_time();
        initialize_service(fsInitialize);
        initialize_service(pmdmntInitialize);

        // Not needed anymore.
        smExit();
    }

    void __appExit()
    {
        pmdmntExit();
        fsExit();
    }
}

int main()
{
    JKSysV jksysv{};
    while (jksysv.is_running()) { jksysv.update(); }
}