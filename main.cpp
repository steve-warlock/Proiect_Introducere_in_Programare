#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <string>


namespace fs = std::filesystem;

void drawFiles(sf::RenderWindow& window, const std::string& directory, float startX, float startY) {
    float y = startY; // Initial position for file listing
    sf::Font font;
    if (!font.loadFromFile("resources/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);

    for (const auto& entry : fs::directory_iterator(directory)) {
        text.setString(entry.path().filename().string());
        text.setPosition(startX, y);
        window.draw(text);
        y += 30.0f;
    }
}

void drawSidebar(sf::RenderWindow& window, const std::string& leftDirectory, const std::string& rightDirectory) {
    sf::Font font;
    if (!font.loadFromFile("resources/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);

    // Left panel sidebar
    std::string leftDiskSpaceInfo = "Free space (Left Panel): ";
    try {
        fs::space_info leftSpace = fs::space(leftDirectory);
        std::size_t leftFreeSpaceGB = leftSpace.available / (1024 * 1024 * 1024); // Convert bytes to GB
        leftDiskSpaceInfo += std::to_string(leftFreeSpaceGB) + " GB out of " + std::to_string(leftSpace.capacity / (1024 * 1024 * 1024)) + " GB";
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        leftDiskSpaceInfo += "Unavailable";
    }

    text.setString(leftDiskSpaceInfo);
    text.setPosition(50.0f, 10.0f); // Position the left sidebar at the top
    window.draw(text);

    // Right panel sidebar
    std::string rightDiskSpaceInfo = "Free space (Right Panel): ";
    try {
        fs::space_info rightSpace = fs::space(rightDirectory);
        std::size_t rightFreeSpaceGB = rightSpace.available / (1024 * 1024 * 1024); // Convert bytes to GB
        rightDiskSpaceInfo += std::to_string(rightFreeSpaceGB) + " GB out of " + std::to_string(rightSpace.capacity / (1024 * 1024 * 1024)) + " GB";
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        rightDiskSpaceInfo += "Unavailable";
    }

    text.setString(rightDiskSpaceInfo);
    text.setPosition(660.0f, 15.0f); // Position the right sidebar at the top
    window.draw(text);
}

void Check_OS(std::string& os)
{
     
    #ifdef _WIN32
        os = "Windows";
    #elif __APPLE__
        os = "macOS";
    #elif __linux__
        os = "Linux";
    #endif
}

int main() {
    std::string leftDirectory;
    std::string rightDirectory;
    std::string osName = "";
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Total Commander", sf::Style::Titlebar | sf::Style::Default | sf::Style::Close);

    Check_OS(osName);
    
    if(!osName.compare("Linux") || !osName.compare("macOS"))
    {
        leftDirectory = "//"; // Left panel directory
        rightDirectory = "//"; // Right panel directory
    }
    else
    {
        leftDirectory = "C:\\";
        rightDirectory = "D:\\";
    }
    

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Draw the sidebar displaying free disk space for both panels
        drawSidebar(window, leftDirectory, rightDirectory);

        // Draw files in the left panel
        drawFiles(window, leftDirectory, 60.0f, 50.0f);

        // Draw files in the right panel
        drawFiles(window, rightDirectory, 650.0f, 50.0f); // Starting X-position for the right panel adjusted

        window.display();
    }

    return 0;
}

