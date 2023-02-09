#include "helpers/fs.h"
#include <nfd.hpp>

#include "helpers/logger.h"
#include "nfd.h"

namespace fs {
    bool OpenFileDialog(DialogMode mode, const DialogFilter &filters, std::function<void(path)> cb, const std::string &default_path)
    {
        NFD::Init();
        nfdresult_t result;
        NFD::UniquePath p;

        switch (mode) {
        case DialogMode::Open:
            result = NFD::OpenDialog(p, filters.data(), filters.size(), default_path.c_str());
            break;
        case DialogMode::Save:
            result = NFD::SaveDialog(p, filters.data(), filters.size(), default_path.c_str(), NULL);
            break;
        default:
            return false;
        }

        if (result == NFD_OKAY)
        {
            cb(p.get());
        }
        else if (result == NFD_ERROR)
        {
            TS_LOG_ERROR("NFD Error: %s", NFD_GetError());
        }

        NFD::Quit();
        return result == NFD_OKAY;
    }
}
