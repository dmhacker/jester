#include "engine/game_engine.hpp"
#include "cfrm/cfrm_storage.hpp"

using namespace jester;

int main() {
    /* GameEngine engine; */ 
    /* engine.shell(); */
    CFRMStorage storage;
    std::cout << "Training is in progress." << std::endl;
    while (true) {
        storage.cfrm().iterate(true);
        storage.save();
        std::cout << "Saved to disk!" << std::endl;
    }
}
