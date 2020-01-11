#ifndef JESTER_CFRM_STORAGE_HPP
#define JESTER_CFRM_STORAGE_HPP

#include "tabular_cfrm.hpp"

namespace jester {

class CFRMStorage {
private:
    std::string d_filename;
    TabularCFRM d_cfrm;

public:
    CFRMStorage(const std::string& filename = "cfrm.bin");
    ~CFRMStorage();

    TabularCFRM& cfrm();
    void save();
};

inline TabularCFRM& CFRMStorage::cfrm() {
    return d_cfrm;
}

}

#endif
