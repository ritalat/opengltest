#include "path.hh"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

const Path BASE_DIR = [](){
    const char *env = getenv("GLLELU_DATADIR");
    if (env)
        return env;
    return ".";
}();

Path datadir()
{
    return BASE_DIR;
}

Path get_path(std::string_view file)
{
    return BASE_DIR / file;
}

Path get_asset_path(std::string_view file)
{
    return BASE_DIR / "assets" / file;
}

Path get_shader_path(std::string_view file)
{
    return BASE_DIR / "shaders" / file;
}

std::string slurp_file(Path path)
{
    std::ifstream stream(path);
    return std::string( (std::istreambuf_iterator<char>(stream)),
                        (std::istreambuf_iterator<char>()) );
}
