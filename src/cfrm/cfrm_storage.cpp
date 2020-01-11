#include "cfrm_storage.hpp"

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"

#include <cereal/archives/portable_binary.hpp>

#include <fstream>

namespace jester {

CFRMStorage::CFRMStorage(const std::string& filename)
    : d_filename(filename)
{
    std::ifstream infile(filename);
    if (infile.good()) {
        cereal::PortableBinaryInputArchive iarchive(infile);
        iarchive(d_cfrm);
    }
}

CFRMStorage::~CFRMStorage()
{
    save();
}

void CFRMStorage::save()
{
    std::ofstream outfile(d_filename);
    if (outfile.good()) {
        cereal::PortableBinaryOutputArchive oarchive(outfile);
        oarchive(d_cfrm);
    }
    else {
        throw std::runtime_error("Unable to save CFRM to disk.");
    }
}

}
