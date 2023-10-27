#pragma once

#include <string>
#include <string_view>

std::string get_path(std::string_view file);
std::string get_asset_path(std::string_view file);
std::string get_shader_path(std::string_view file);
std::string slurp_file(std::string_view path);
