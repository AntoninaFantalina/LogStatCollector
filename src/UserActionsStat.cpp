#include "UserActionsStat.h"
#include "rapidjson/document.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"

#include <time.h>
#include <iterator>
#include <sstream>

namespace {
    auto toDate(uint32_t timestamp) {
        constexpr uint32_t sec_in_day = 60 * 60 * 24;
        return timestamp - (timestamp % sec_in_day);
    }

    auto toFormattedDate(uint32_t timestamp) {
        time_t datetime(timestamp);
        tm* tm_ = localtime(&datetime);
        return std::to_string(tm_->tm_year + 1900) + "-" +
               std::to_string(tm_->tm_mon + 1) + "-" +
               std::to_string(tm_->tm_mday);;
    }
}

bool PropsKey::operator==(const PropsKey& rhs) const {
    return values == rhs.values;
}

std::string PropsKey::toString() const {
    std::string result;
    if (!values.empty()) {
        for (size_t i = 0; i < values.size() - 1; ++i) {
            result += std::to_string(values[i]) + ",";
        }
        result += std::to_string(values.back());
    }
    return result;
}

void UserActionsStat::addUserAction(const UserAction& action) {
    ++data_[toDate(action.ts_fact)][action.fact_name][PropsKey{action.props}];
}

void UserActionsStat::add(const UserActionsStat& other_stat) {
    for (const auto& [date, other_action_stat] : other_stat.data_) {
        auto& date_stat = data_[date];
        for (const auto& [action_name, other_props_stat] : other_action_stat) {
            auto& action_stat = date_stat[action_name];
            for (const auto& [prop, count] : other_props_stat) {
                action_stat[prop] += count;
            }
        }
    }
}

void UserActionsStat::serialize(std::ostream& out_stream) const {
    using namespace rapidjson;

    Document doc; 
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    for (const auto& [date, action_stat] : data_) {
        Value json_action(Type::kObjectType);
        for (const auto& [action_name, props_stat] : action_stat) {
            Value json_props(Type::kObjectType);
            for (const auto& [prop, count] : props_stat) {
                json_props.AddMember(
                    Value(prop.toString().c_str(), allocator),
                    Value(static_cast<int>(count)), 
                    allocator);
            }
            json_action.AddMember(
                Value(action_name.c_str(), allocator), 
                std::move(json_props), 
                allocator);
        }
        doc.AddMember(
            Value(toFormattedDate(date).c_str(), allocator),
            std::move(json_action), 
            allocator);
    }

    OStreamWrapper osw(out_stream);
    PrettyWriter<OStreamWrapper> writer(osw);
    writer.SetIndent(' ', 2);
    doc.Accept(writer);
}
