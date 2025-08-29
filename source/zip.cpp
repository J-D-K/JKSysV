#include "zip.hpp"

#include "fslib.hpp"

#include <memory>

namespace
{
    /// @brief This is much more limited than what full-blown JKSV uses.
    constexpr int SIZE_FILE_BUFFER = 0x80000;
}

void copy_directory_to_zip(const fslib::Path &source, MiniZip &zip)
{
    fslib::Directory sourceDir{source};
    if (!sourceDir.is_open()) { return; }

    for (const fslib::DirectoryEntry &entry : sourceDir.list())
    {
        const fslib::Path fullSource{source / entry};
        if (entry.is_directory()) { copy_directory_to_zip(source, zip); }
        else
        {
            fslib::File sourceFile{fullSource, FsOpenMode_Read};
            const bool newZipFile = zip.open_new_file(fullSource.full_path());
            if (!sourceFile.is_open() || !newZipFile) { continue; }

            auto fileBuffer        = std::make_unique<unsigned char[]>(SIZE_FILE_BUFFER);
            const int64_t fileSize = sourceFile.get_size();
            for (int64_t i = 0; i < fileSize;)
            {
                const ssize_t read = sourceFile.read(fileBuffer.get(), SIZE_FILE_BUFFER);
                if (read == -1) { break; }

                const bool write = zip.write(fileBuffer.get(), read);
                if (!write) { break; }

                i += read;
            }
            zip.close_current_file();
        }
    }
}