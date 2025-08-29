#include "ScopedMount.hpp"

#include "fslib.hpp"
#include "paths.hpp"

ScopedMount::ScopedMount(uint64_t applicationID, AccountUid accountID)
{
    m_isOpen = fslib::open_account_save_file_system(paths::SAVE_MOUNT, applicationID, accountID);
}

ScopedMount::~ScopedMount()
{
    if (m_isOpen) { fslib::close_file_system(paths::SAVE_MOUNT); }
}

bool ScopedMount::is_open() const { return m_isOpen; }