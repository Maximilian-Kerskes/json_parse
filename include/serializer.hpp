#pragma once

#include "value.hpp"

#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>

    namespace json_private {

    inline void write_string(std::ostream & out, const std::string &value) {

        out << '"';

        for (char c : value) {
            switch (c) {
            case '"':
                out << "\\\"";
                break;
            case '\\':
                out << "\\\\";
                break;
            case '\n':
                out << "\\n";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\t':
                out << "\\t";
                break;
            default:
                out << c;
                break;
            }
        }

        out << '"';
    }

    inline void write_value(std::ostream & out, const Json &value) {
        value.visit([&](const auto &item) {
            using T = std::decay_t<decltype(item)>;

            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                out << "null";
            } else if constexpr (std::is_same_v<T, bool>) {
                out << (item ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                write_string(out, item);
            } else if constexpr (std::is_same_v<T, Json::array>) {
                out << '[';

                for (std::size_t i = 0; i < item.size(); ++i) {
                    if (i > 0) {
                        out << ',';
                    }

                    write_value(out, item[i]);
                }

                out << ']';
            } else if constexpr (std::is_same_v<T, Json::object>) {
                out << '{';

                std::size_t i = 0;

                for (const auto &[key, child] : item) {
                    if (i++ > 0) {
                        out << ',';
                    }

                    write_string(out, key);
                    out << ':';
                    write_value(out, child);
                }

                out << '}';
            } else {
                out << item;
            }
        });
    }

} // namespace json_private

inline std::string serialize(const Json &value) {
    std::ostringstream out;
    json_private::write_value(out, value);
    return out.str();
}

inline std::ostream &operator<<(std::ostream &out, const Json &value) {
    json_private::write_value(out, value);
    return out;
}
