#include "ArgParser.h"
#include "CollectStat.h"

#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

std::vector<fs::path> createPaths(
    const fs::path& input_dir, size_t files_count) {
    std::vector<fs::path> paths;
    paths.reserve(files_count);
    for (size_t i = 0; i < files_count; ++i) {
        fs::path path = input_dir / ("file" + std::to_string(i + 1) + ".log");
        if (!fs::exists(path)) {
            throw std::runtime_error("File " + path.generic_string() + " does not exist");
        }
        paths.push_back(std::move(path));
    }
    return paths;
}

int main(int argc, const char** argv) {
    try {
        auto start = std::chrono::system_clock::now();
        ArgParser arg_parser(argc, argv);
        arg_parser.AddParam("--input_dir", "Path to input directory", true, true);
        arg_parser.AddParam("--files", "Files count", true, true);
        arg_parser.AddParam("--threads", "Threads count", false, true);
        arg_parser.AddParam("--output_path", "Path to output file", true, true);
        arg_parser.AddParam("--help", "Print help");
        arg_parser.parse();
        if (arg_parser.has("--help")) {
            arg_parser.printHelp(std::cout);
            return 0;
        }

        fs::path input_dir(*arg_parser.get("--input_dir"));
        size_t files_count = std::stoul(std::string(arg_parser.get("--files")->data()));
        size_t threads_count = 1;
        if (auto threads = arg_parser.get("--threads"); threads.has_value()) {
            threads_count = std::stoul(std::string(threads->data()));
        }
        fs::path output_path(*arg_parser.get("--output_path"));
        std::vector<fs::path> input_paths = createPaths(input_dir, files_count);
        UserActionsStat common_stat = collectActionsStat(input_paths, threads_count);

        std::ofstream stream(output_path);
        if (!stream) {
            throw std::runtime_error("Can't open file " + output_path.generic_string());
        }
        common_stat.serialize(stream);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "Elapsed time: " << elapsed_seconds.count() << " sec" << std::endl;
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
