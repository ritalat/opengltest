#pragma once

#include "api_decl.hh"

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

MYLLY_API Path datadir();
MYLLY_API Path assetdir();
MYLLY_API Path shaderdir();
MYLLY_API Path getPath(std::string_view file);
MYLLY_API Path getAssetPath(std::string_view file);
MYLLY_API Path getShaderPath(std::string_view file);
MYLLY_API std::string slurpFile(const Path &path);
