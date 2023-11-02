#pragma once

#include <filesystem>
#include <string>
#include <string_view>

using Path = std::filesystem::path;

Path datadir();
Path get_path(std::string_view file);
Path get_asset_path(std::string_view file);
Path get_shader_path(std::string_view file);
std::string slurp_file(Path path);
