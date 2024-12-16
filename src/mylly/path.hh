#pragma once

#include <filesystem>
#include <string>
#include <string_view>

using Path = std::filesystem::path;

#define FILE_ERROR_HINT "\n\nHint: Assets and shaders are searched from the current working directory\n" \
                        "You can change the search path by setting GLLELU_DATADIR"

#ifdef WIN32
// Windows std path is internally wchar :/
#define cpath(path) path.string().data()
#else
#define cpath(path) path.c_str()
#endif

Path datadir();
Path assetdir();
Path shaderdir();
Path getPath(std::string_view file);
Path getAssetPath(std::string_view file);
Path getShaderPath(std::string_view file);
std::string slurpFile(const Path &path);
