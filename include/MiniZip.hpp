#pragma once
#include "fslib.hpp"

#include <minizip/zip.h>
#include <string_view>

class MiniZip final
{
    public:
        MiniZip() = default;

        /// @brief Constructor. Calls open upon construction;
        /// @param path
        MiniZip(const fslib::Path &path);

        /// @brief Closes the zipFile.
        ~MiniZip();

        /// @brief Returns whether or not the zip file was successfully opened.
        /// @return
        bool is_open() const;

        /// @brief Opens a Zip file at path
        bool open(const fslib::Path &path);

        /// @brief Manual call for closing the zipFile.
        void close();

        /// @brief Opens a new file with filename as the path.
        bool open_new_file(std::string_view filename, bool trimPath = false, size_t trimPlaces = 0);

        /// @brief Closes the currently open file in the Zip.
        bool close_current_file();

        /// @brief Attempts to write the buffer passed to the currently opened file.
        bool write(const void *buffer, size_t dataSize);

    private:
        /// @brief Stores whether or not the zipFile was opened successfully.
        bool m_isOpen{};

        /// @brief Underlying ZIP file.
        zipFile m_zip{};
};
