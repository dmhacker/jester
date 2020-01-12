#include "engine/game_engine.hpp"
#include "cfrm/cfrm_storage.hpp"

using namespace jester;

int main() {
    std::cout << "!!! WARNING !!! Using a reduced form of Durak at the moment!" << std::endl;
    std::cout << "!!! WARNING !!! Only ranks 6-7 are being used." << std::endl;
    std::cout << "!!! WARNING !!! This is for CFRM training purposes." << std::endl;
    std::cout << std::endl;

    /* CFRMStorage storage; */
    /* std::cout << "Training is in progress." << std::endl; */
    /* for (size_t i = 0; i < 200; i++) { */
    /*     storage.cfrm().iterate(); */
    /*     std::cout << std::endl << "Saving in progress." << std::endl; */
    /*     storage.save(); */
    /*     std::cout << "Saved to disk." << std::endl; */
    /* } */
    GameEngine engine; 
    engine.shell();
}
