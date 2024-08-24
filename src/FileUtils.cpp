#include "FileUtils.hpp"

#include <fstream>

std::string getFullPath(const std::string& filename)
{
    const std::filesystem::path cwd = std::filesystem::current_path();
    const std::filesystem::path paragraph_dir = cwd.parent_path().parent_path()
                                                / "assets";
    std::string filename_ = paragraph_dir.string() + "/" + filename;
    return filename_;
}