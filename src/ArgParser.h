#include <iostream>
#include <map>
#include <optional>
#include <string_view>

class ArgParser {
public:
    ArgParser(int argc, const char** argv);

    void AddParam(
        std::string_view name, 
        std::string_view description, 
        bool required = false,
        bool has_value = false);

    void printHelp(std::ostream& stream) const;

    void parse();

    bool has(std::string_view name) const;

    std::optional<std::string_view> get(std::string_view name) const;

private:
    struct Attributes {
        std::string_view description;
        bool required;
        bool has_value;
    };

    std::optional<std::string_view> nextArg();

    int argc_;
    const char** argv_;
    const char** cur_arg_;

    std::map<std::string_view, Attributes> expected_params_;
    std::map<std::string_view, std::optional<std::string_view>> fact_params_;
};
