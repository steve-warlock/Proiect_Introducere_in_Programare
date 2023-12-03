//
//  the_app.cpp
//  Total_Commander
//
//  Created by Steve Warlock on 02.12.2023.
//

#include "the_app.hpp"


namespace app {
TotalCommander::TotalCommander() :window(sf::VideoMode(1280,720), "Total Commander"){
    
}

TotalCommander::~TotalCommander()
{
    
}

void TotalCommander::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            std::cout << "Handling Event Closed" << '\n';
            exit(EXIT_SUCCESS);
        }
        // Handle other events like mouse clicks, keyboard inputs, etc.
    }
}

void TotalCommander::update() {
    // Update logic for the application
}

void TotalCommander::render() {
    window.clear(sf::Color::White);
    // Render GUI components and file system representation using SFML
    // Use GUIManager and FileManager to display content
    window.display();
}

void TotalCommander::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

}
