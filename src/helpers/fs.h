#pragma once

#include "nfd.h"
#include <filesystem>
#include <functional>
#include <system_error>
#include <vector>
#include <nfd.hpp>

namespace fs
{
    using path = std::filesystem::path;

    enum DialogMode : char
    {
        Open,
        Save
    };

    using DialogFilter = std::vector<nfdfilteritem_t>;
    bool OpenFileDialog(DialogMode mode, const DialogFilter &filters, std::function<void(path)> cb, const std::string &default_path = {});

    static inline uintmax_t GetFileSize(const path &path)
    {
        std::error_code err;
        auto size = std::filesystem::file_size(path);
        if (err) return 0;
        return size;
    }

    static inline bool IsRegularFile(const path &path)
    {
        return std::filesystem::is_regular_file(path);
    }

    static inline bool Exists(const path &path)
    {
        return std::filesystem::exists(path);
    }

    static inline bool Remove(const path &path)
    {
        return std::filesystem::remove(path);
    }
}
