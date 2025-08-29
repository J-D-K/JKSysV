#include "JKSysV.hpp"

#include "MiniZip.hpp"
#include "ScopedMount.hpp"
#include "json.hpp"
#include "paths.hpp"
#include "stringutil.hpp"
#include "zip.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <thread>

namespace
{
    // Paths to grab config data from.
    constexpr const char *PATH_JKSYSV_CONFIG = "sdmc:/config/JKSV/JKSysV.json";
    constexpr const char *PATH_JKSV_CONFIG   = "sdmc:/config/JKSV/JKSV.json";

    // Time to sleep between scans in seconds.
    constexpr int64_t TIME_SLEEP_SECONDS = 1;
}

JKSysV::JKSysV()
{
    const bool fslib    = fslib::initialize();
    const bool fslibdev = fslib::dev::initialize_sdmc();

    const bool appIDs  = JKSysV::load_application_ids();
    const bool workDir = JKSysV::load_jksv_work_dir();
    if (!fslib || !fslibdev || !appIDs || !workDir) { return; }

    m_isRunning = true;
}

JKSysV::~JKSysV() { fslib::exit(); }

bool JKSysV::is_running() const { return m_isRunning; }

void JKSysV::update()
{
    JKSysV::scan_application_ids();
    JKSysV::scan_for_exit();

    std::this_thread::sleep_for(std::chrono::seconds(TIME_SLEEP_SECONDS));
}

bool JKSysV::load_application_ids()
{
    static constexpr const char *KEY_APPLICATION_IDS = "applicationIDs";

    json::Object config = json::new_object(json_object_from_file, PATH_JKSYSV_CONFIG);
    if (!config) { return false; }

    json_object *appIDArray = json::get_object(config, KEY_APPLICATION_IDS);
    if (!appIDArray) { return false; }

    const int arrayLength = json_object_array_length(appIDArray);
    for (int i = 0; i < arrayLength; i++)
    {
        json_object *element = json_object_array_get_idx(appIDArray, i);
        if (!element) { break; }

        const char *hexString        = json_object_get_string(element);
        const uint64_t applicationID = std::strtoull(hexString, nullptr, 16);
        m_applicationIDs.push_back(applicationID);
    }

    return true;
}

bool JKSysV::load_jksv_work_dir()
{
    static constexpr const char *KEY_WORK_DIR = "WorkingDirectory";

    json::Object config = json::new_object(json_object_from_file, PATH_JKSV_CONFIG);
    if (!config) { return false; }

    json_object_iterator current = json::iter_begin(config);
    json_object_iterator end     = json::iter_end(config);
    while (!json_object_iter_equal(&current, &end))
    {
        const char *name         = json_object_iter_peek_name(&current);
        json_object *valueObject = json_object_iter_peek_value(&current);

        const bool workingDir = std::strcmp(name, KEY_WORK_DIR) == 0;
        if (workingDir)
        {
            m_workingDirectory = json_object_get_string(valueObject);
            return true;
        }
        json_object_iter_next(&current);
    }
    return false;
}

void JKSysV::scan_application_ids()
{
    if (m_processRunning) { return; }

    for (const uint64_t &applicationID : m_applicationIDs)
    {
        uint64_t processID{};
        const bool isRunning = R_SUCCEEDED(pmdmntGetProcessId(&processID, applicationID));
        if (isRunning)
        {
            m_currentProcess = applicationID;
            m_processRunning = true;
            break;
        }
    }
}

void JKSysV::scan_for_exit()
{
    if (!m_processRunning) { return; }

    uint64_t processID{};
    const bool stillRunning = R_SUCCEEDED(pmdmntGetProcessId(&processID, m_currentProcess));
    if (stillRunning) { return; }

    m_processRunning = false;
    JKSysV::create_new_backup();
}

void JKSysV::create_new_backup()
{
    static constexpr const char *FORMAT_BACKUP  = "JKSysV - %Y-%m-%d_%H-%M-%S.zip";
    static constexpr int64_t SLEEP_TIME_SECONDS = 5;

    // Need to sleep to give the game time to shutdown and actually close, otherwise mounting isn't possible.
    std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME_SECONDS));

    AccountUid accountID{};
    bool lastUser = R_SUCCEEDED(accountGetLastOpenedUser(&accountID));
    if (!lastUser) { return; }

    const std::string idHex = stringutil::get_formatted_string("%016llX", m_currentProcess);
    const fslib::Path targetDir{m_workingDirectory / idHex};

    const bool exists  = fslib::directory_exists(targetDir);
    const bool created = !exists && fslib::create_directory(targetDir);
    if (!exists && !created) { return; }

    char backupName[0x80]    = {0};
    std::time_t rawTime      = std::time(nullptr);
    const std::tm *localTime = std::localtime(&rawTime);
    std::strftime(backupName, 0x80, FORMAT_BACKUP, localTime);

    const fslib::Path backupPath{targetDir / backupName};
    ScopedMount saveMount{m_currentProcess, accountID};
    if (!saveMount.is_open()) { return; }

    MiniZip backup{backupPath};
    if (!backup.is_open()) { return; }

    copy_directory_to_zip(paths::SAVE_ROOT, backup);
}
