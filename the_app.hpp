//
//  the_app.hpp
//  Total_Commander
//
//  Created by Steve Warlock on 02.12.2023.
//

#pragma once

//sfml libraries
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>





//std
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
//#include <>

//graphics
#include "the_graphic.hpp"

//functions
#include "the_functions.hpp"

//bonus ;)
#include "verification.hpp"



namespace app{

class TotalCommander {
private:
    sf::RenderWindow window;
    FileManager fileManager;
    GUIManager guiManager;
    
public:
    TotalCommander();
    ~TotalCommander();
    void run();
    
private:
    void handleEvents();
    void update();
    void render(); 
};

}
