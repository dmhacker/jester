#ifndef JESTER_LOGGING_HPP
#define JESTER_LOGGING_HPP

#include <deque>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jester {

extern std::shared_ptr<spdlog::logger> bots_logger;
extern std::shared_ptr<spdlog::logger> training_logger;

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container)
{
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& container)
{
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& container)
{
    os << "{";
    size_t i = 0;
    for (auto& item : container) {
        os << item;
        if (i < container.size() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "}";
    return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& container)
{
    os << "{";
    size_t i = 0;
    for (auto& item : container) {
        os << item.first << " : " << item.second;
        if (i < container.size() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "}";
    return os;
}

}

#endif
