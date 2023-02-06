#include "CollectStat.h"
#include "rapidjson/document.h"

#include <iostream>
#include <fstream>
#include <optional>
#include <mutex>
#include <thread>
#include <string_view>

namespace fs = std::filesystem;

namespace {
    auto generatePropNames() {
        std::array<std::string, PropsCount> names;
        for (size_t i = 0; i < names.size(); ++i) {
            names[i] = "prop" + std::to_string(i + 1);
        }
        return names;
    }

    auto userActionFromJson(std::string_view str) {
        using namespace rapidjson;

        static const auto prop_names = generatePropNames();
        Document doc;
        doc.Parse(str.data());
        UserAction action;
        action.fact_name = doc["fact_name"].GetString();
        const Value& json_props = doc["props"];
        for (size_t i = 0; i < action.props.size(); ++i) {
            action.props[i] = json_props[prop_names[i].c_str()].GetUint();
        }
        action.actor_id = doc["actor_id"].GetUint();
        action.ts_fact = doc["ts_fact"].GetUint();
        return action;
    }
}

class IndexManager {
public:
    explicit IndexManager(size_t size)
        : size_(size) {
    }

    std::optional<size_t> next() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (task_index_ < size_) {
            return task_index_++;
        }
        return std::nullopt;
    }

private:
    std::mutex mutex_;
    size_t task_index_ = 0;
    size_t size_ = 0;
};

auto collectActionsStat(const fs::path& path) {
    std::cout << "Thread " << std::this_thread::get_id()  << " process " << path << std::endl;
    std::ifstream stream(path);
    if (!stream) {
        std::cout << "Can't open file " + path.generic_string();
    }

    UserActionsStat stat;
    std::string line;
    while (std::getline(stream, line)) {
        stat.addUserAction(userActionFromJson(line));
    }
    return stat;
}

void collectFilesActionsStat(
    const std::vector<fs::path>& paths,
    std::vector<UserActionsStat>& stats,
    IndexManager& manager) noexcept {
    try {
        while (auto index = manager.next()) {
            stats[index.value()] = collectActionsStat(paths.at(index.value()));
        }
    }
    catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
    }
}

UserActionsStat collectActionsStat(const std::vector<fs::path>& paths, size_t threads_count) {
    std::vector<UserActionsStat> stats(paths.size());
    IndexManager manager(paths.size());

    threads_count = std::min(threads_count, paths.size());
    std::vector<std::thread> threads;
    threads.reserve(threads_count - 1);
    for (size_t i = 0; i < threads_count - 1; ++i) {
        threads.emplace_back(collectFilesActionsStat, std::cref(paths), std::ref(stats), std::ref(manager));
    }
    collectFilesActionsStat(paths, stats, manager);
    for (auto& thread : threads) {
        thread.join();
    }

    UserActionsStat common_stat;
    for (const auto& stat : stats) {
        common_stat.add(stat);
    }
    return common_stat;
}
