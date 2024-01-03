#include <windows.h>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <ShlObj.h>
#include "winbgim.h"
#include "graphics.h"
#define Nr_of_Buttons 5
using namespace std;

string left_panel_path = "D:\\";
string right_panel_path = "C:\\";



struct node
{
    string name;
    string extension;
    double size_file;
    string modified_date;
    bool is_directory;
    node* next;
    node* prev;
};

bool which_panel = true; // true - stanga, false - dreapta
node* currentSelectedLeft = nullptr;
node* currentSelectedRight = nullptr;

bool tabPressed = false; // Variabilă de comutare pentru a permite o singură schimbare între panouri

bool pathExists(const std::string& path)
{
    DWORD attributes = GetFileAttributes(path.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}


void display_path(int x1, int y1, int x2, int y2, std::string& path)
{
    settextstyle(4, 0, 11);
    setcolor(COLOR(151, 159, 165));
    setbkcolor(COLOR(47, 61, 76));

    if (!pathExists(path.c_str()))
    {
        std::cout << "Path is not valid or doesn't exist\n";
        // Handle accordingly, for example, displaying an error message
        return;
    }

    size_t lastSlashPos = path.find_last_of("\\");

    if (lastSlashPos != std::string::npos)
    {
        std::string lastFolder = path.substr(lastSlashPos + 1);

        int textWidth = textwidth((char*)lastFolder.c_str());

        if (textWidth > (x2 - x1 - 4))
        {
            int visibleLength = x2 - x1 - 4; // To leave space for ".."

            if (visibleLength > 0)
            {
                std::string truncatedPath = ".." + path.substr(path.length() - visibleLength + 2);
                outtextxy(x1 + 2, y1 + 7, (char*)truncatedPath.c_str());
                return;
            }
        }
    }

    int ok = 0;
    string mod_path = path;
    while (textwidth((char*)mod_path.c_str()) > 610)
    {
        mod_path.pop_back();
        ok = 1;
    }
    if (ok)
        mod_path += "...";


    outtextxy(x1 + 2, y1 + 7, (char*)mod_path.c_str());
}




void DrawArrow(int x1, int y1, int x2, int y2, const std::string& buttonText)
{
    settextstyle(4, 0, 15);
    setcolor(COLOR(47, 61, 76));
    int textX = (x1 + x2) / 2 - textwidth((char*)buttonText.c_str()) / 2;
    int textY = (y1 + y2) / 2 - textheight((char*)buttonText.c_str()) / 2;
    settextstyle(4, 0, 12);
    setbkcolor(COLOR(151, 159, 165));
    outtextxy(textX, textY, (char*)buttonText.c_str());
}




void DrawShortcutButton(int x1, int y1, int x2, int y2, const std::string& buttonText)
{
    settextstyle(4, 0, 12);
    setcolor(COLOR(151, 159, 165));
    int textX = (x1 + x2) / 2 - textwidth((char*)buttonText.c_str()) / 2;
    int textY = (y1 + y2) / 2 - textheight((char*)buttonText.c_str()) / 2;
    setbkcolor(COLOR(47, 61, 76));
    outtextxy(textX, textY, (char*)buttonText.c_str());
}

void DrawButton(int x1, int y1, int x2, int y2, const std::string& buttonText)
{
    settextstyle(4, 0, 12);
    setcolor(COLOR(47, 61, 76));
    int textX = (x1 + x2) / 2 - textwidth((char*)buttonText.c_str()) / 2;
    int textY = (y1 + y2) / 2 - textheight((char*)buttonText.c_str()) / 2;
    settextstyle(4, 0, 12);
    setbkcolor(COLOR(151, 159, 165));
    outtextxy(textX, textY, (char*)buttonText.c_str());
}



void free_space(const std::string& current_path, int x1, int y1, int x2, int y2)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL success = GetDiskFreeSpaceEx(
        current_path.c_str(),
        &freeBytesAvailable,
        &totalNumberOfBytes,
        &totalNumberOfFreeBytes
    );

    if (success)
    {
        std::string availableSpaceStr = std::to_string(freeBytesAvailable.QuadPart / (1024ULL * 1024ULL * 1024ULL)) +
            " GB free of " +
            std::to_string(totalNumberOfBytes.QuadPart / (1024ULL * 1024ULL * 1024ULL)) +
            " GB";

        // Display text in the graphics window
        DrawShortcutButton(x1, y1, x2, y2, availableSpaceStr.c_str());
    }
    else
    {
        std::cerr << "Error getting disk space information.\n";
    }
}


void shortcut_buttons()
{
    // comenzi rapide
    DrawShortcutButton(0, 690, 252, 720, "F2 Rename");
    DrawShortcutButton(252, 690, 504, 720, "F3 Mkdir");
    DrawShortcutButton(504, 690, 756, 720, "F4 Copy");
    DrawShortcutButton(756, 690, 1008, 720, "F5 Delete");
    DrawShortcutButton(1008, 690, 1280, 720, "F6 Move");

    //liniile dintre butoanele comenzilor rapide de jos
    bar(252, 690, 257, 720);
    bar(509, 690, 514, 720);
    bar(766, 690, 771, 720);
    bar(1023, 690, 1028, 720);
    bar(615, 30, 617, 60);
    bar(1255, 30, 1257, 60);

    //iconita muzica
    setfillstyle(SOLID_FILL, COLOR(151, 159, 165));
    bar(617, 662, 640, 690);
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(623, 684, 3, 3);
    fillellipse(633, 682, 3, 3);
    bar(624, 684, 626, 668);
    bar(634, 682, 636, 666);
    line(626, 668, 635, 666);
    line(626, 669, 635, 667);

    
    //lupe
    setfillstyle(SOLID_FILL, COLOR(220, 220, 220));
    fillellipse(629, 43, 7, 7);
    fillellipse(1269, 43, 7, 7);

    setfillstyle(SOLID_FILL, COLOR(118, 118, 118));
    bar(627, 49, 631, 57);
    bar(1267, 49, 1271, 57);


}

void draw_first_panel()
{
    //sidebar de sus (display the memory)
    free_space(left_panel_path, 0, 0, 640, 30);

    //display the path
    display_path(0, 30, 640, 60, left_panel_path);



}

void draw_second_panel()
{
    //title bar de sus (display the memory)
    free_space(right_panel_path, 640, 0, 1280, 30);

    //display the path
    display_path(642, 30, 1280, 60, right_panel_path);


}

void draw_zones()
{
    cleardevice();
    setbkcolor(WHITE);
    setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
    bar(0, 0, 1280, 60);
    bar(0, 660, 1280, 720);
    setfillstyle(SOLID_FILL, COLOR(151, 159, 165));
    bar(0, 60, 615, 660);
    bar(640, 60, 1255, 660);
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 30, 1280, 32);
    bar(0, 60, 1280, 64);
    bar(0, 690, 1280, 692);
    bar(0, 720, 1280, 724);
    bar(640, 0, 642, 690);
    bar(615, 60, 617, 690);
    bar(1255, 60, 1257, 660);
    bar(0, 90, 1280, 94);
    bar(0, 630, 1280, 632);
    bar(0, 660, 1280, 662);

    draw_first_panel();
    //butoanele de deasupra fisierelor panel 1
    bar(270, 60, 273, 90);
    bar(355, 60, 358, 90);
    bar(440, 60, 443, 90);
    //denumiri panel 1
    DrawButton(0, 60, 279, 90, "Name");
    DrawButton(280, 60, 335, 90, "Ext");
    DrawButton(355, 60, 440, 90, "Size");
    DrawButton(440, 60, 615, 90, "Date");
    DrawArrow(624, 79, 640, 90, "^");
    DrawArrow(626, 647, 640, 660, "v");

    //liniile ce delimiteaza informatiile
    bar(270, 90, 273, 630);
    bar(355, 90, 358, 630);
    bar(440, 90, 443, 630);

    draw_second_panel();
    // butoanele de deasupra fisierelor panel 2
    bar(910, 60, 913, 90);
    bar(995, 60, 998, 90);
    bar(1080, 60, 1083, 90);

    //denumiri panel 2
    DrawButton(660, 60, 910, 90, "Name");
    DrawButton(910, 60, 995, 90, "Ext");
    DrawButton(995, 60, 1080, 90, "Size");
    DrawButton(1080, 60, 1255, 90, "Date");

    //liniile ce delimiteaza informatiile
    bar(910, 90, 913, 630);
    bar(995, 90, 998, 630);
    bar(1080, 90, 1083, 630);
    DrawArrow(1264, 79, 1280, 90, "^");
    DrawArrow(1266, 647, 1280, 660, "v");

    shortcut_buttons();
}

node* getFileList(const std::string& path) {
    if (!pathExists(path)) {
        std::cout << "Path is not valid or doesn't exist\n";
        return nullptr;
    }

    std::string wildcardPath = path + "\\*";
    WIN32_FIND_DATA fileData;
    HANDLE hFind = FindFirstFile(wildcardPath.c_str(), &fileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "Error finding the first file\n";
        return nullptr;
    }

    node* head = nullptr;
    node* current = nullptr;
    bool isFirstFile = false;

    do {
        std::string fileName = fileData.cFileName;
        if (fileName.empty())
        {
                fileName = "no_name";   
        }
        if (fileName != ".") {
            node* newNode = new node;
            newNode->name = fileName;

            // Verifică dacă este un director
            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                newNode->size_file = -1; // Placeholder pentru directoare
                // newNode->extension = "<DIR>";
            }
            else {
                // Extrage extensia și calculează dimensiunea în megabytes pentru fișiere
                size_t pos = newNode->name.find_last_of('.');
                if (pos != std::string::npos) {
                    newNode->extension = newNode->name.substr(pos + 1);
                    newNode->name = newNode->name.substr(0, pos);
                }
                else {
                    newNode->extension = ""; // Nu există extensie
                }
                newNode->size_file = static_cast<double>(fileData.nFileSizeLow) / (1024 * 1024);
            }

            // Converteste FILETIME la string de timp local pentru data modificării
            FILETIME lastWriteTime = fileData.ftLastWriteTime;
            SYSTEMTIME sysTime;
            FileTimeToSystemTime(&lastWriteTime, &sysTime);
            char modifiedDate[128] = { '\0' };
            GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, nullptr, modifiedDate, 128);
            newNode->modified_date = modifiedDate;

            newNode->is_directory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            newNode->next = nullptr;

            if (!isFirstFile) {
                head = newNode;
                current = newNode;
                isFirstFile = true;
            }
            else {
                current->next = newNode;
                newNode->prev = current; // Actualizează pointerul anterior
                current = newNode;
            }
        }
    } while (FindNextFile(hFind, &fileData) != 0);

    FindClose(hFind);
    return head;
}





void printFileDetailsLeftPanel(node* fileList, int startX, int startY)
{
    settextstyle(3, 0, 11); // Set text style
    setcolor(COLOR(4, 4, 4)); // Set text color
    setbkcolor(COLOR(151, 159, 165)); // Set bk color

    while (fileList != nullptr && startY <= 600)
    {
        // Numele fișierului
        std::string fileName = fileList->name;
        if (!fileName.empty())
        {
            int ok = 0;
            while(textwidth((char*)fileName.c_str())>244)
            {
                fileName.pop_back();
                ok = 1;
            }
            if(ok)
                fileName += "...";
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileName.c_str()) / 2;
            outtextxy(5, textY, (char*)fileName.c_str());
        }

        // Extensia fișierului
        std::string fileExtension = fileList->extension;
        if (!fileExtension.empty() && fileExtension != "")
        {
            int textX = (270 + 355) / 2 - textwidth((char*)fileExtension.c_str()) / 2;
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileExtension.c_str()) / 2;
            outtextxy(textX, textY, (char*)fileExtension.c_str());
        }

        // Tipul de fișier (director sau fișier)
        if (fileList->is_directory)
        {
            std::string fileType = "<DIR>";
            int textX = (355 + 440) / 2 - textwidth((char*)fileType.c_str()) / 2;
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileType.c_str()) / 2;
            outtextxy(textX, textY, (char*)fileType.c_str());
        }
        else
        {
            // Dimensiunea fișierului
            char fileSizeStr[256] = {'\0'};
            snprintf(fileSizeStr, sizeof(fileSizeStr), "%.2f MB", fileList->size_file);
            int textX = (355 + 440) / 2 - textwidth(fileSizeStr) / 2;
            int textY = (startY + startY + 30) / 2 - textheight(fileSizeStr) / 2;
            outtextxy(textX, textY, fileSizeStr);
        }

        // Data modificării
        std::string modifiedDate = fileList->modified_date;
        int textX = (440 + 635) / 2 - textwidth((char*)modifiedDate.c_str()) / 2;
        int textY = (startY + startY + 30) / 2 - textheight((char*)modifiedDate.c_str()) / 2;
        outtextxy(textX, textY, (char*)modifiedDate.c_str());

        startY += 25; // Increment Y position for the next file details
        fileList = fileList->next;
    }
}


void printFileDetailsRightPanel(node* fileList, int startX, int startY)
{
    settextstyle(3, 0, 11); // Set text style
    setcolor(COLOR(4, 4, 4)); // Set text color
    setbkcolor(COLOR(151, 159, 165)); // Set bk color

    while (fileList != nullptr && startY <= 600)
    {
        // Numele fișierului
        std::string fileName = fileList->name;
        if (!fileName.empty())
        {
            bool ok = false;
            while (textwidth((char*)fileName.c_str()) > 244)
            {
                fileName.pop_back();
                ok = true;
            }
            if (ok)
                fileName += "...";
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileName.c_str()) / 2;
            outtextxy(645, textY, (char*)fileName.c_str());
        }

        // Extensia fișierului
        std::string fileExtension = fileList->extension;
        if (!fileExtension.empty() && fileExtension != "")
        {
            int textX = (910 + 995) / 2 - textwidth((char*)fileExtension.c_str()) / 2;
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileExtension.c_str()) / 2;
            outtextxy(textX, textY, (char*)fileExtension.c_str());
        }

        // Tipul de fișier (director sau fișier)
        if (fileList->is_directory)
        {
            std::string fileType = "<DIR>";
            int textX = (995 + 1080) / 2 - textwidth((char*)fileType.c_str()) / 2;
            int textY = (startY + startY + 30) / 2 - textheight((char*)fileType.c_str()) / 2;
            outtextxy(textX, textY, (char*)fileType.c_str());
        }
        else
        {
            // Dimensiunea fișierului
            char fileSizeStr[256];
            snprintf(fileSizeStr, sizeof(fileSizeStr), "%.2f MB", fileList->size_file);
            int textX = (995 + 1080) / 2 - textwidth(fileSizeStr) / 2;
            int textY = (startY + startY + 30) / 2 - textheight(fileSizeStr) / 2;
            outtextxy(textX, textY, fileSizeStr);
        }

        // Data modificării
        std::string modifiedDate = fileList->modified_date;
        int textX = (1080 + 1255) / 2 - textwidth((char*)modifiedDate.c_str()) / 2;
        int textY = (startY + startY + 30) / 2 - textheight((char*)modifiedDate.c_str()) / 2;
        outtextxy(textX, textY, (char*)modifiedDate.c_str());

        startY += 25; // Increment Y position for the next file details
        fileList = fileList->next;
    }
}


void Free_Memory(node* filelist) {
    while (filelist != nullptr) {
        node* temp = filelist;
        filelist = filelist->next;
        delete temp;
    }
}


// Funcție pentru accesarea folderului și actualizarea căii
void AccessFolder(std::string& currentPath, node*& selectedNode, node*& which_panel_files) {
    if (selectedNode == nullptr || !selectedNode->is_directory) {
        return; // Do nothing if the node is not a directory or is null
    }

    if (selectedNode->name != "..") {
        currentPath += selectedNode->name;
        currentPath += "\\";
        Free_Memory(which_panel_files);
        which_panel_files = getFileList(currentPath);
        selectedNode = which_panel_files;
    }
    else {
        if (currentPath.length() > 3) {
            size_t pos = currentPath.find_last_of("\\", currentPath.length() - 3);
            currentPath = currentPath.substr(0, pos + 1);
            Free_Memory(which_panel_files);
            which_panel_files = getFileList(currentPath);
            selectedNode = which_panel_files;
        }
    }
}


// Funcție pentru deschiderea fișierului
void OpenFile(const std::string& currentPath, node* selectedNode) {
    if (selectedNode != nullptr) {
        std::string fullPath = currentPath + selectedNode->name + "." + selectedNode->extension;
        ShellExecuteA(nullptr, "open", fullPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}


void highlightItem(node* fileList, int startX, int startY)
{
    int textWidth = textwidth((char*)fileList->name.c_str()) + 8;
    int textHeight = textheight((char*)fileList->name.c_str()) + 2;
    int y = startY + 5; // Offset pentru a se potrivi cu textul

    setcolor(COLOR(47, 61, 76)); // Setează culoarea la albastru transparent
    setfillstyle(SOLID_FILL, COLOR(47, 61, 76)); // Setează umplerea la albastru transparent

    rectangle(startX - 1, y, startX + 262, y + textHeight + 1); // Desenează dreptunghiul pentru zona de selecție
    rectangle(startX - 2, y - 1, startX + 263, y + textHeight + 2);
    rectangle(startX - 3, y - 2, startX + 264, y + textHeight + 3);
}

void clearSelectionHighlight(node* fileList, int startX, int startY)
{
    int textWidth = textwidth((char*)fileList->name.c_str()) + 8;
    int textHeight = textheight((char*)fileList->name.c_str()) + 2;
    int y = startY + 5; // Offset pentru a se potrivi cu textul

    setcolor(COLOR(151, 159, 165)); // Setează culoarea la culoarea de fundal a ferestrei
    setfillstyle(SOLID_FILL, COLOR(151, 159, 165)); // Setează umplerea la culoarea de fundal a ferestrei

    rectangle(startX - 1, y, startX + 262, y + textHeight + 1); // Desenează dreptunghiul pentru zona de selecție
    rectangle(startX - 2, y - 1, startX + 263, y + textHeight + 2);
    rectangle(startX - 3, y - 2, startX + 264, y + textHeight + 3);
}

void switchPanel(node*& currentSelected, node* panelFiles, int& modifiableY, int startX) {
    clearSelectionHighlight(currentSelected, startX, modifiableY);
    currentSelected = panelFiles;
    modifiableY = 93;
    highlightItem(currentSelected, 648 - startX, modifiableY);
}

int countFiles(const node* file_path) {
    int count = 0;
    const node* temp = file_path;

    // Traverse forward and count nodes
    while (temp != nullptr) {
        ++count;
        temp = temp->next;
    }

    return count;
}




bool isPointInInterval(int x, int y, int startX, int startY, int endX, int endY)
{
    return (x >= startX && x <= endX && y >= startY && y <= endY);
}


bool compareNodesByName(const node* a, const node* b) {
    return a->name < b->name;
}

bool compareNodesByExtension(const node* a, const node* b) {
    return a->extension < b->extension;
}

bool compareNodesBySize(const node* a, const node* b) {
    return a->size_file < b->size_file;
}

bool compareNodesByDate(const node* a, const node* b) {
    return a->modified_date < b->modified_date;
}

void sort_by(const string& what_criteria, node*& panelnodes) {
    if (panelnodes == nullptr || panelnodes->next == nullptr) {
        // Handling cases of empty list or list with a single node (already sorted)
        return;
    }

    // Store pointers to nodes in a vector, excluding the first node ".."
    vector<node*> nodes;
    node* current = panelnodes->next; // Start from the second node
    while (current != nullptr) {
        if (current->name != "..") {
            nodes.push_back(current);
        }
        current = current->next;
    }

    // Sort the vector based on the specified criteria and in descending order
    if (what_criteria == "Name") {
        sort(nodes.begin(), nodes.end(), compareNodesByName);
    }
    else if (what_criteria == "Ext") {
        sort(nodes.begin(), nodes.end(), compareNodesByExtension);
    }
    else if (what_criteria == "Size") {
        sort(nodes.begin(), nodes.end(), compareNodesBySize);
    }
    else if (what_criteria == "Date") {
        sort(nodes.begin(), nodes.end(), compareNodesByDate);
    }

    // Reconstruct the linked list using the sorted nodes
    panelnodes->next = nullptr; // Set panelnodes to ".."
    if (!nodes.empty()) {
        for (size_t i = 0; i < nodes.size() - 1; ++i) {
            nodes[i]->next = nodes[i + 1];
            nodes[i + 1]->prev = nodes[i];
        }
        nodes.front()->prev = panelnodes;
        nodes.back()->next = nullptr;
        panelnodes->next = nodes.front();
    }

    // Clean up memory by deleting nodes
    for (node* n : nodes) {
        delete n;
    }
}

void popup_error_window(const string& action)
{




}



void open_popup_window(const std::string& action, std::string& input) {

}







//function keys implementation
void Rename_File(node*& selectednode, const string& new_name, const string& given_path) {
    string old_file_name = given_path + selectednode->name + "." + selectednode->extension;
    string new_file_name = given_path + new_name + "." + selectednode->extension;

    if (MoveFile(old_file_name.c_str(), new_file_name.c_str())) {
        // Update the node's name if the file rename was successful
        selectednode->name = new_name;
        cout << "File renamed successfully!" << endl;
    }
    else {
        cerr << "Error renaming file!" << endl;
    }
}


void Rename_Directory(node*& selectednode, const string& new_name, const string& given_path) {
    string old_dir_name = given_path + selectednode->name;
    string new_dir_name = given_path + new_name;

    if (MoveFileEx(old_dir_name.c_str(), new_dir_name.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        // Update the node's name if the directory rename was successful
        selectednode->name = new_name;
        cout << "Directory renamed successfully!" << endl;
    }
    else {
        cerr << "Error renaming directory!" << endl;
    }
}


void create_directory(node*& panelnode, const std::string& name) {
    if (CreateDirectory(name.c_str(), NULL)) {
        if (ERROR_ALREADY_EXISTS == GetLastError()) {
            std::cout << "Folder already exists!" << std::endl;
            //displayErrorPopup("Folder already exists!");
        }
        else {
            std::cout << "Folder created successfully!" << std::endl;
        }

        // Get the last write time of the directory
        WIN32_FIND_DATA fileData;
        HANDLE hDir = FindFirstFile((name + "/*").c_str(), &fileData);
        if (hDir != INVALID_HANDLE_VALUE) {
            FILETIME lastWriteTime = fileData.ftLastWriteTime;
            FindClose(hDir);

            // Convert last write time to SYSTEMTIME
            SYSTEMTIME sysTime;
            FileTimeToSystemTime(&lastWriteTime, &sysTime);

            // Format as short date string
            char modifiedDate[128] = { '\0' };
            GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, nullptr, modifiedDate, 128);

            // Create a new node for the directory
            node* newDirectory = new node();
            newDirectory->name = name;
            newDirectory->is_directory = true;
            newDirectory->modified_date = modifiedDate;
            newDirectory->next = nullptr;

            // Find the last node in the list
            node* current = panelnode;
            while (current->next != nullptr) {
                current = current->next;
            }

            // Add newDirectory after the last node
            current->next = newDirectory;
            newDirectory->prev = current;
        }
        else {
            std::cout << "Failed to get last write time for the folder." << std::endl;
        }
    }
    else {
        std::cout << "Failed to create folder. Error: " << GetLastError() << std::endl;
    }
}

void copyFile(const std::string& path, node*& selectednode, const std::string& destinationFolder) {
    std::string sourceFileName = selectednode->name + "." + selectednode->extension;
    std::string sourceFilePath = path + sourceFileName;
    std::string destinationPath = destinationFolder + "\\" + sourceFileName;

    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(sourceFilePath, destinationPath, options);

        std::filesystem::file_time_type sourceTime = std::filesystem::last_write_time(sourceFilePath);
        std::filesystem::last_write_time(destinationPath, sourceTime);

        std::cout << "File copied successfully!" << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Failed to copy file: " << e.what() << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}


void copyDirectory(const std::string& path, node*& selectednode, const std::string& destinationFolder) {
    std::string sourceFolderPath = path + selectednode->name; // Assuming selectednode->name is the folder name
    std::string destinationPath = destinationFolder + "\\" + selectednode->name; // Assuming selectednode->name is the folder name

    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(sourceFolderPath, destinationPath, options);

        std::cout << "Folder copied successfully!" << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Failed to copy folder: " << e.what() << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}


void deleteFile(const string& basePath, node* selectednode, node*& panelnodes) {
    std::string filePath = basePath + selectednode->name + "." + selectednode->extension;
    if (DeleteFileA(filePath.c_str()) != 0) {
        std::cout << "File deleted successfully!" << std::endl;

        // Remove the selected node from the linked list
        if (selectednode == panelnodes) {
            panelnodes = panelnodes->next;
            if (panelnodes != nullptr) {
                panelnodes->prev = nullptr;
            }
            delete selectednode;
            return;
        }

        if (selectednode->next != nullptr) {
            selectednode->next->prev = selectednode->prev;
        }
        selectednode->prev->next = selectednode->next;
        delete selectednode;
    }
    else {
        std::cout << "Failed to delete file. Error code: " << GetLastError() << std::endl;
    }
}


void deleteDirectory(const std::string& basePath, node* selectednode, node*& panelnodes) {
    std::string directoryPath = basePath + selectednode->name;
    if (RemoveDirectoryA(directoryPath.c_str()) != 0) {
        std::cout << "Directory deleted successfully!" << std::endl;

        // Remove the selected node from the linked list
        if (selectednode == panelnodes) {
            panelnodes = panelnodes->next;
            if (panelnodes != nullptr) {
                panelnodes->prev = nullptr;
            }
            delete selectednode;
            return;
        }

        if (selectednode->next != nullptr) {
            selectednode->next->prev = selectednode->prev;
        }
        selectednode->prev->next = selectednode->next;
        delete selectednode;
    }
    else {
        std::cout << "Failed to delete directory. Error code: " << GetLastError() << std::endl;
    }
}

void moveFile(const std::string& basePath, node* selectednode, node*& panelnodes, const std::string& destinationFolder) {
    copyFile(basePath, selectednode, destinationFolder); // Copy file to destination
    deleteFile(basePath, selectednode, panelnodes); // Delete original file
}

void moveDirectory(const std::string& basePath, node* selectednode, node*& panelnodes, const std::string& destinationFolder) {
    copyDirectory(basePath, selectednode, destinationFolder); // Copy directory to destination
    deleteDirectory(basePath, selectednode, panelnodes); // Delete original directory
}


int main()
{


    initwindow(1290, 730, "Total Commander");

    node* leftPanelFiles = getFileList(left_panel_path);
    node* rightPanelFiles = getFileList(right_panel_path);
    currentSelectedLeft = leftPanelFiles;
    currentSelectedRight = rightPanelFiles;
    int maxim1 = -1; // pentru a ramane constant nuamrul de fisiere in stanga
    int maxim2 = -1; // pentru a ramane constant numarul de fisiere in dreapta
    int startY = 93;
    int modifiable_left_Y = startY;
    int modifiable_right_Y = startY;

    float ysus1 = 97, ysus2 = 97;
    float yjos1 = 0, yjos2 = 0;

    int page = 0;
    while (true)
    {

        setactivepage(page);
        cleardevice();

        draw_zones();

        if (leftPanelFiles != nullptr)
        {
            // Display file details in the WinBGIm window
            printFileDetailsLeftPanel(leftPanelFiles, 0, startY); // Modify X position as needed
        }
        else
        {
            // Handle accordingly if file lists are not retrieved
            std::cerr << "Error retrieving file lists.\n";
        }
        if (rightPanelFiles != nullptr)
        {

            printFileDetailsRightPanel(rightPanelFiles, 650, startY); // Modify X position as needed
        }
        else
        {
            std::cerr << "Error retrieving file lists.\n";
        }

        if (which_panel)
            highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
        else
            highlightItem(currentSelectedRight, 644, modifiable_right_Y);



        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            closegraph();
            Free_Memory(currentSelectedLeft);
            Free_Memory(currentSelectedRight);
            Free_Memory(leftPanelFiles);
            Free_Memory(rightPanelFiles);
            exit(0);
        }

        if (GetAsyncKeyState(VK_TAB) & 0x8000) {
            if (which_panel) {
                which_panel = false;
                switchPanel(currentSelectedLeft, leftPanelFiles, modifiable_left_Y, 4);
            }
            else {
                which_panel = true;
                switchPanel(currentSelectedRight, rightPanelFiles, modifiable_right_Y, 644);
            }
            Sleep(70);
        }

        //scrollbar
        int nrfis1 = countFiles(leftPanelFiles);
        maxim1 = max(maxim1, nrfis1);
        double latim1 = (double)540 / maxim1;
        if (abs((double)latim1 - (int)latim1) > 0.5)
            latim1 = (int)latim1 + 0.5;
        else
            latim1 = (int)latim1;

        int nrfis2 = countFiles(rightPanelFiles);
        maxim2 = max(maxim2, nrfis2);
        double latim2 = (int)540 / maxim2 + 0.5;

        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            if (which_panel && currentSelectedLeft->prev)
            {
                if (modifiable_left_Y - 25 > 90)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y -= 25;
                    currentSelectedLeft = currentSelectedLeft->prev;
                    printFileDetailsLeftPanel(leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    if (maxim1 > 21 && ysus1 - latim1 > 92 && leftPanelFiles->prev && currentSelectedLeft->prev)
                        ysus1 -= latim1, leftPanelFiles = leftPanelFiles->prev, currentSelectedLeft = currentSelectedLeft->prev;
                }
            }
            else if (!which_panel && currentSelectedRight->prev)
            {
                if (modifiable_right_Y - 25 > 90)
                {
                    clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                    modifiable_right_Y -= 25;
                    currentSelectedRight = currentSelectedRight->prev;
                    printFileDetailsRightPanel(rightPanelFiles, 644, startY);
                    highlightItem(currentSelectedRight, 644, modifiable_right_Y);
                }
                else
                {
                    if (maxim2 > 21 && ysus2 - latim2 > 92 && rightPanelFiles->prev && currentSelectedRight->prev)
                        ysus2 -= latim2, rightPanelFiles = rightPanelFiles->prev, currentSelectedRight = currentSelectedRight->prev;
                }
            }
            Sleep(50);
        }
        //panelul selectat
        setcolor(COLOR(47, 61, 76));
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if(which_panel)
            fillellipse(294, 645, 6, 6);
        else
            fillellipse(954, 645, 6, 6);
        setcolor(COLOR(47, 61, 76));
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if (which_panel)
            fillellipse(308, 645, 6, 6);
        else
            fillellipse(968, 645, 6, 6);
        setcolor(COLOR(47, 61, 76));
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if (which_panel)
            fillellipse(322, 645, 6, 6);
        else
            fillellipse(982, 645, 6, 6);

        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            if (which_panel && currentSelectedLeft->next)
            {
                if (modifiable_left_Y + 25 < 610)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y += 25;
                    currentSelectedLeft = currentSelectedLeft->next;
                    printFileDetailsLeftPanel(leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    if (maxim1 > 21 && ysus1 + 21 * latim1 < 630 && leftPanelFiles->next && currentSelectedLeft->next)
                        ysus1 += latim1, leftPanelFiles = leftPanelFiles->next, currentSelectedLeft = currentSelectedLeft->next;
                }
            }
            else if (!which_panel && currentSelectedRight->next)
            {
                if (modifiable_right_Y + 25 < 610)
                {
                    clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                    modifiable_right_Y += 25;
                    currentSelectedRight = currentSelectedRight->next;
                    printFileDetailsRightPanel(rightPanelFiles, 644, startY);
                    highlightItem(currentSelectedRight, 644, modifiable_right_Y);
                }
                else
                {
                    if (maxim2 > 21 && ysus2 + 21 * latim2 < 630 && rightPanelFiles->next && currentSelectedRight->next)
                        ysus2 += latim2, rightPanelFiles = rightPanelFiles->next, currentSelectedRight = currentSelectedRight->next;
                }
            }
            Sleep(50);
        }


        if (ismouseclick(WM_LBUTTONDOWN))
        {
            int mouseX = 0, mouseY = 0;
            getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

            //scrollbar
            if (isPointInInterval(mouseX, mouseY, 615, 60, 640, 90))
                if (maxim1 > 21 && ysus1 - latim1 > 92 && leftPanelFiles->prev && currentSelectedLeft->prev)
                    ysus1 -= latim1, leftPanelFiles = leftPanelFiles->prev, currentSelectedLeft = currentSelectedLeft->prev;
            if (isPointInInterval(mouseX, mouseY, 615, 630, 640, 660))
                if (maxim1 > 21 && ysus1 + 21 * latim1 < 630 && leftPanelFiles->next && currentSelectedLeft->next)
                    ysus1 += latim1, leftPanelFiles = leftPanelFiles->next, currentSelectedLeft = currentSelectedLeft->next;

            if (isPointInInterval(mouseX, mouseY, 1255, 60, 1280, 90))
                if (maxim2 > 21 && ysus2 - latim2 > 92 && rightPanelFiles->prev && currentSelectedRight->prev)
                    ysus2 -= latim2, rightPanelFiles = rightPanelFiles->prev, currentSelectedRight = currentSelectedRight->prev;
            if (isPointInInterval(mouseX, mouseY, 1255, 630, 1280, 660))
                if (maxim2 > 21 && ysus2 + 21 * latim2 < 630 && rightPanelFiles->next && currentSelectedRight->next)
                    ysus2 += latim2, rightPanelFiles = rightPanelFiles->next, currentSelectedRight = currentSelectedRight->next;


            //comenzile rapide
            if (isPointInInterval(mouseX, mouseY, 0, 690, 252, 720))
            { //F2
                int window2 = initwindow(500, 250, "Rename");
                setcurrentwindow(window2);
                setbkcolor(WHITE);
                setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
                bar(0, 0, 500, 250);
                getch();
                closegraph();
            }
            if (isPointInInterval(mouseX, mouseY, 253, 690, 504, 720))
            { //F3
                initwindow(500, 250, "MkDir");
                setbkcolor(WHITE);
                setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
                bar(0, 0, 500, 250);
                getch();
                closegraph();
            }
            if (isPointInInterval(mouseX, mouseY, 505, 690, 756, 720))
            { //F4
                initwindow(500, 250, "Copy");
                setbkcolor(WHITE);
                setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
                bar(0, 0, 500, 250);
                getch();
                closegraph();
            }

            if (isPointInInterval(mouseX, mouseY, 1009, 690, 1280, 720))
            { //F6
                initwindow(500, 250, "Move");
                setbkcolor(WHITE);
                setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
                bar(0, 0, 500, 250);
                while (!kbhit())
                {
                    delay(100);
                }
                closegraph();
            }

            Sleep(70);
        }

        if (maxim1 < 22)
            yjos1 = 627;
        else
            yjos1 = ysus1 + 21 * latim1;

        if (maxim2 < 22)
            yjos2 = 627;
        else
            yjos2 = ysus2 + 21 * latim2;
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if (which_panel && !currentSelectedLeft->next)
            yjos1 = 627;
        if (!which_panel && !currentSelectedRight->next)
            yjos2 = 627;
        if (yjos1 > 627)
            yjos1 = 627;
        if (yjos2 > 627)
            yjos2 = 627;
        bar(620, ysus1, 637, yjos1);
        bar(1260, ysus2, 1277, yjos2);


        if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            if (which_panel) {
                if (currentSelectedLeft != nullptr) {
                    if (currentSelectedLeft->is_directory) {
                        AccessFolder(left_panel_path, currentSelectedLeft, leftPanelFiles);
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        modifiable_left_Y = startY;
                        highlightItem(leftPanelFiles, 4, modifiable_left_Y);
                        maxim1 = -1;
                        ysus1 = 97, ysus2 = 97;
                        yjos1 = 0, yjos2 = 0;
                    }
                    else {
                        OpenFile(left_panel_path, currentSelectedLeft);
                    }
                }
            }
            else {
                if (currentSelectedRight != nullptr) {
                    if (currentSelectedRight->is_directory) {
                        clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                        AccessFolder(right_panel_path, currentSelectedRight, rightPanelFiles);
                        modifiable_right_Y = startY;
                        highlightItem(rightPanelFiles, 644, modifiable_right_Y);
                        maxim2 = -1;
                        ysus1 = 97, ysus2 = 97;
                        yjos1 = 0, yjos2 = 0;
                    }
                    else {
                        OpenFile(right_panel_path, currentSelectedRight);
                    }
                }
            }
            Sleep(50);
        }


        if (GetAsyncKeyState(VK_F2) & 0x8000)
        {
            string new_name = "";
            open_popup_window("Rename to:", new_name);
            if (which_panel)
            {
                if (!currentSelectedLeft->is_directory)
                    Rename_File(currentSelectedLeft, new_name, left_panel_path);
                else
                    Rename_Directory(currentSelectedLeft, new_name, left_panel_path);
                sort_by("Name", leftPanelFiles);
            }
            else
            {
                if (!currentSelectedRight->is_directory)
                    Rename_File(currentSelectedRight, new_name, right_panel_path);
                else
                    Rename_Directory(currentSelectedRight, new_name, right_panel_path);
                sort_by("Name", rightPanelFiles);
            }
            new_name.clear();
            Sleep(25);
        }

        if (GetAsyncKeyState(VK_F3) & 0x8000)
        {
            string new_name = "";
            open_popup_window("Name the directory to:", new_name);
            if (which_panel)
                create_directory(leftPanelFiles, new_name);
            else
                create_directory(rightPanelFiles, new_name);

            new_name.clear();
        }

        if (GetAsyncKeyState(VK_F4) & 0x8000)
        {
            string destinationfolder = "";
            open_popup_window("Where to:", destinationfolder);

            if (which_panel)
            {

                if (!currentSelectedLeft->is_directory)
                    copyFile(left_panel_path, currentSelectedLeft, destinationfolder);
                else
                    copyDirectory(left_panel_path, currentSelectedLeft, destinationfolder);
            }
            else
            {
                if (!currentSelectedRight->is_directory)
                    copyFile(right_panel_path, currentSelectedRight, destinationfolder);
                else
                    copyDirectory(right_panel_path, currentSelectedRight, destinationfolder);
            }
            destinationfolder.clear();
            Sleep(25);
        }

        if (GetAsyncKeyState(VK_F5) & 0x8000)
        {
            if (which_panel)
            {
                if (!currentSelectedLeft->is_directory)
                    deleteFile(left_panel_path, currentSelectedLeft, leftPanelFiles);
                else
                    if (currentSelectedLeft->name == "..")
                        popup_error_window("Failed to delete the directory!");
                    else
                        deleteDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles);
            }
            else
            {
                if (!currentSelectedRight->is_directory)
                    deleteFile(right_panel_path, currentSelectedRight, rightPanelFiles);
                else
                    deleteDirectory(right_panel_path, currentSelectedRight, rightPanelFiles);
            }
            Sleep(25);
        }

        if (GetAsyncKeyState(VK_F6) & 0x8000)
        {
            string destinationFolder = "";
            open_popup_window("Move to:", destinationFolder);
            if (which_panel)
            {
                if (!currentSelectedLeft->is_directory)
                    moveFile(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder);
                else
                    moveDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder);
            }
            else
            {
                if (!currentSelectedRight->is_directory)
                    moveFile(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder);
                else
                    moveDirectory(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder);
            }
            destinationFolder.erase(destinationFolder.begin(), destinationFolder.end());
            Sleep(25);
        }

        //delay(50);
        Sleep(50);
        setvisualpage(page);
        page = 1 - page;

    }

    closegraph();
    Free_Memory(currentSelectedLeft);
    Free_Memory(currentSelectedRight);
    Free_Memory(leftPanelFiles);
    Free_Memory(rightPanelFiles);


    return 0;
}
