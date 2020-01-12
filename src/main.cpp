#include "engine/game_engine.hpp"
#include "cfrm/cfrm_storage.hpp"

using namespace jester;

int main() {
    CFRMStorage storage;
    std::cout << "Training is in progress." << std::endl;
    for (size_t i = 0; i < 200; i++) {
        storage.cfrm().iterate();
        storage.save();
        std::cout << std::endl << "Saved to disk!" << std::endl;
    }
    /* GameEngine engine; */ 
    /* engine.shell(); */
}
