#include <iosfwd>
#include <unordered_map>
#include <functional>
#include <array>
#include <string>

constexpr size_t PropsCount = 10;

struct UserAction {
    using PropValues = std::array<uint32_t, PropsCount>;

    std::string fact_name;
    PropValues props;
    uint32_t actor_id;
    uint32_t ts_fact;
};

struct PropsKey {
    bool operator==(const PropsKey& rhs) const;
    std::string toString() const;

    UserAction::PropValues values;
};

template <class T>
inline void hash_combine(size_t& seed, const T& v) // from boost
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
    template <>
    struct hash<PropsKey>
    {
        size_t operator()(const PropsKey& props) const
        {
            size_t seed = 100017;
            for (const auto value : props.values) {
                hash_combine(seed, value);
            }
            return seed;
        }
    };
}

class UserActionsStat {
public:
    using PropsToCount = std::unordered_map<PropsKey, size_t>;
    using ActionKey = std::string;
    using ActionToProps = std::unordered_map<ActionKey, PropsToCount>;
    using Date = uint32_t;

    void addUserAction(const UserAction& action);

    void add(const UserActionsStat& other_stat);

    void serialize(std::ostream& s) const;

private:
    std::unordered_map<Date, ActionToProps> data_;
};

