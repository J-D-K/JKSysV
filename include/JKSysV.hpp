#pragma once
#include "fslib.hpp"

#include <cstdint>
#include <vector>

class JKSysV final
{
    public:
        JKSysV();

        ~JKSysV();

        // None of this nonsense!
        JKSysV(const JKSysV &)            = delete;
        JKSysV(JKSysV &&)                 = delete;
        JKSysV &operator=(const JKSysV &) = delete;
        JKSysV &operator=(JKSysV &&)      = delete;

        /// @brief Returns whether or not JKSysV is running.
        bool is_running() const;

        /// @brief JKSysV update routine.
        void update();

    private:
        /// @brief Stores whether or not JKSysV is running.
        bool m_isRunning{};

        /// @brief Stores whether a process is running or not.
        bool m_processRunning{};

        /// @brief Stores the application ID of the currently running process.
        uint64_t m_currentProcess{};

        /// @brief Stores the current working directory.
        fslib::Path m_workingDirectory{};

        /// @brief Application IDs to track.
        std::vector<uint64_t> m_applicationIDs{};

        /// @brief Loads the application ids from the file JKSV creates.
        bool load_application_ids();

        /// @brief Opens JKSV's config and loads it's current working directory.
        bool load_jksv_work_dir();

        /// @brief Scans to see if any of the application IDs in the array are actively running.
        void scan_application_ids();

        /// @brief Checks to see if the title was exited.
        void scan_for_exit();

        /// @brief Creates a new backup for the title.
        void create_new_backup();
};