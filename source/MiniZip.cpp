#include "MiniZip.hpp"

#include <ctime>

// Definition at bottom.
static zip_fileinfo create_zip_file_info();

MiniZip::MiniZip(const fslib::Path &path) { MiniZip::open(path); }

MiniZip::~MiniZip() { MiniZip::close(); }

bool MiniZip::is_open() const { return m_isOpen; }

bool MiniZip::open(const fslib::Path &path)
{
    MiniZip::close();
    m_zip = zipOpen64(path.full_path(), APPEND_STATUS_CREATE);
    if (!m_zip) { return false; }
    m_isOpen = true;
    return true;
}

void MiniZip::close()
{
    if (!m_isOpen) { return; }
    zipClose(m_zip, nullptr);
    m_isOpen = false;
}

bool MiniZip::open_new_file(std::string_view filename, bool trimPath, size_t trimPlaces)
{
    const size_t pathBegin = filename.find_first_of('/');
    if (pathBegin != filename.npos) { filename = filename.substr(pathBegin + 1); }

    const zip_fileinfo fileInfo = create_zip_file_info();
    return zipOpenNewFileInZip64(m_zip, filename.data(), &fileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, 6, 0) ==
           ZIP_OK;
}

bool MiniZip::close_current_file() { return zipCloseFileInZip(m_zip) == ZIP_OK; }

bool MiniZip::write(const void *buffer, size_t dataSize) { return zipWriteInFileInZip(m_zip, buffer, dataSize) == ZIP_OK; }

static zip_fileinfo create_zip_file_info()
{
    const std::time_t currentTime = std::time(nullptr);
    const std::tm *local          = std::localtime(&currentTime);
    const zip_fileinfo fileInfo   = {.tmz_date    = {.tm_sec  = local->tm_sec,
                                                     .tm_min  = local->tm_min,
                                                     .tm_hour = local->tm_hour,
                                                     .tm_mday = local->tm_mday,
                                                     .tm_mon  = local->tm_mon,
                                                     .tm_year = local->tm_year + 1900},
                                     .dosDate     = 0,
                                     .internal_fa = 0,
                                     .external_fa = 0};

    return fileInfo;
}
