#pragma once
#include <cstdint>
#include <switch.h>

class ScopedMount final
{
    public:
        /// @brief Creates a new ScopedMount.
        ScopedMount(uint64_t applicationID, AccountUid accountID);

        /// @brief Closes the mounted system.
        ~ScopedMount();

        /// @brief Returns if the scoped mount was opened successfully.
        /// @return
        bool is_open() const;

    private:
        /// @brief Stores if the mount was successfully opened.
        bool m_isOpen{};
};