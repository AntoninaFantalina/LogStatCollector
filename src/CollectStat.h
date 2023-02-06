#include "UserActionsStat.h"

#include <vector>
#include <filesystem>

UserActionsStat collectActionsStat(const std::vector<std::filesystem::path>& paths, size_t threads_count = 1);
