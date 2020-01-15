#ifndef JESTER_PRINTING_HPP
#define JESTER_PRINTING_HPP

#include <iostream>
#include <deque>

#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>

namespace jester {

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
