#include "core/Game.hpp"

#include <cstdlib>

int main() {
    Game game;

    game.initializer();
    game.run();
    game.shutdown();

    return EXIT_SUCCESS;
}
