#ifndef STDA_ERASED_PTR_HPP
#define STDA_ERASED_PTR_HPP

#include <functional>
#include <memory>

/*
 * Taken from https://akrzemi1.wordpress.com/2013/12/11/type-erasure-part-iii/
 * This is a unique pointer with a type-erased deleter.
 */

namespace stda {

template <typename T>
std::function<void(void*)> make_erased_deleter()
{
    return {
        [](void* p) {
            delete static_cast<T*>(p);
        }
    };
};

template <typename T>
using erased_ptr = std::unique_ptr<T, std::function<void(void*)>>;

template <typename T, typename ...Args>
erased_ptr<T> make_erased(Args && ...args)
{
    return erased_ptr<T>(new T(args...), make_erased_deleter<T>());
}

}

#endif
