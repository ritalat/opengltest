#pragma once

#include <filesystem>
#include <string>
#include <string_view>

using Path = std::filesystem::path;

#ifdef WIN32
// Windows std path is internally wchar :/
#define cpath(path) path.string().data()
#else
#define cpath(path) path.c_str()
#endif

Path datadir();
Path get_path(std::string_view file);
Path get_asset_path(std::string_view file);
Path get_shader_path(std::string_view file);
std::string slurp_file(const Path &path);
