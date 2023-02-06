#include "ArgParser.h"

ArgParser::ArgParser(int argc, const char** argv)
    : argc_(argc)
    , argv_(argv)
    , cur_arg_(argv + 1) {
}

void ArgParser::AddParam(std::string_view name, std::string_view description, bool required, bool has_value) {
    expected_params_.emplace(name, Attributes{description, required, has_value});
}

void ArgParser::printHelp(std::ostream& stream) const {
    stream << "Options:\n";
    for (const auto& [name, attrs] : expected_params_) {
        stream << "  " << name << " - " << attrs.description << (attrs.required ? "" : " (optional)") << "\n";
    }
}

std::optional<std::string_view> ArgParser::nextArg() {
    if (cur_arg_ == (argv_ + argc_)) {
        return std::nullopt;
    }
    return *cur_arg_++;
}

void ArgParser::parse() {
    while (auto arg = nextArg()) {
        auto key = arg.value();
        if (auto expected = expected_params_.find(key); expected != expected_params_.end()) {
            std::optional<std::string_view> value;
            if (expected->second.has_value) {
                value = nextArg();
                if (value == std::nullopt) {
                    throw std::runtime_error("Missed value for parameter " + std::string(arg->data()));
                }
            }
            fact_params_.emplace(key, value);
        } else {
            throw std::runtime_error("Unexpected parameter " + std::string(arg->data()));
        }
    }

    for (const auto& [name, attrs] : expected_params_) {
        if (attrs.required && !has(name)) {
            throw std::runtime_error("Required parameter " + std::string(name.data()) + " not provided");
        }
    }
}

bool ArgParser::has(std::string_view name) const {
    return fact_params_.find(name) != fact_params_.end();
}

std::optional<std::string_view> ArgParser::get(std::string_view name) const {
    if (auto found = fact_params_.find(name); found != fact_params_.end()) {
        return found->second;
    }
    return std::nullopt;
}
