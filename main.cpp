#include "GameEngine.hpp"

int main()
{
    std::cout << "Hello Assignment 03!" << std::endl;
    GameEngine game("assets.txt");
    game.run();
    return 0;
}
