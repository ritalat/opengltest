#include "path.hh"

#include <fstream>
#include <string>
#include <string_view>

const std::string BASE_DIR = [](){
    const char *env = getenv("GLLELU_DATADIR");
    if (env)
        return env;
    return ".";
}();

std::string get_path(std::string_view file)
{
    return BASE_DIR + "/" + file.data();
}

std::string get_asset_path(std::string_view file)
{
    return BASE_DIR + "/assets/" + file.data();
}

std::string get_shader_path(std::string_view file)
{
    return BASE_DIR + "/shaders/" + file.data();
}

std::string slurp_file(std::string_view path)
{
    std::ifstream stream(path.data());
    return std::string( (std::istreambuf_iterator<char>(stream)),
                        (std::istreambuf_iterator<char>()) );
}
