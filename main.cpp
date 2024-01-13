#include <windows.h>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <cctype>
#include <string>
#include <memory>
#include <iostream>
#include <cctype> // Pentru std::tolower
#include <ShlObj.h>
#include <sstream>
#include <iomanip>
#include <utility>
#include <chrono>
#include "Malware_Analysis.hpp"
#include "winbgim.h"
#include "graphics.h"
#define Nr_of_Buttons 5
using namespace std;

string left_panel_path = "D:\\";
string right_panel_path = "C:\\";



bool which_panel = true; // true - stanga, false - dreapta
node* currentSelectedLeft = new node;
node* currentSelectedRight = new node;


bool tabPressed = false; // Variabilă de comutare pentru a permite o singură schimbare între panouri

bool pathExists(const std::string& path)
{
    DWORD attributes = GetFileAttributes(path.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool pathMusicExists(const std::string& path)
{
    DWORD fileAttributes = GetFileAttributes(path.c_str());

    return (fileAttributes != INVALID_FILE_ATTRIBUTES &&
        !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
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


    int ok = 0;
    string mod_path = path;

    while (textwidth((char*)mod_path.c_str()) > 595)
    {
        mod_path.erase(0, 1); // Elimină primul caracter din șir
        ok = 1;
    }

    if (ok && mod_path.find('\\') != std::string::npos)
    {
        while (mod_path[0] != '\\')
            mod_path.erase(0, 1);
        mod_path.insert(0, "...");
    }

    outtextxy(x1 + 2, y1 + 7, (char*)mod_path.c_str());
}




void DrawArrow(int x1, int y1, int x2, int y2, const std::string& buttonText)
{
    settextstyle(4, 0, 15);
    setcolor(BLACK);
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

    /*if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "Error finding the first file\n";
        return nullptr;
    }*/

    node* head = nullptr;
    node* current = nullptr;
    bool isFirstFile = false;
    int counter = 1;
    do {
        std::string fileName = fileData.cFileName;
        if (fileName.empty())
        {
            fileName = "no_name";
        }
        if (fileName != ".") {
            node* newNode = new node;
            newNode->name = fileName;
            newNode->id = counter++;
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
            while (textwidth((char*)fileName.c_str()) > 244)
            {
                fileName.pop_back();
                ok = 1;
            }
            if (ok)
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
            char fileSizeStr[256] = { '\0' };
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


void Free_Memory(node*& filelist) {
    node* current = filelist;
    while (current != nullptr) {
        node* next = current->next;
        delete current;
        current = next;
    }
    filelist = nullptr;
}


// Funcție pentru accesarea folderului și actualizarea căii
void AccessFolder(std::string& currentPath, node*& selectedNode, node*& which_panel_files) {

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

void AccessFolder(string& currentPath, string& newPath, node*& which_panel_files)
{
    currentPath = newPath;
    Free_Memory(which_panel_files);
    which_panel_files = getFileList(currentPath);
}

// Funcție pentru deschiderea fișierului
void OpenFile(const std::string& currentPath, node* selectedNode) {
    if (selectedNode != nullptr) {
        std::string fullPath = currentPath + selectedNode->name + "." + selectedNode->extension;
        ShellExecuteA(nullptr, "open", fullPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void PlayMusic(const char* MusicPath)
{
    PlaySound(TEXT(MusicPath), NULL, SND_FILENAME | SND_ASYNC);
}

void StopMusic()
{
    PlaySound(NULL, NULL, SND_PURGE);
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

/*void HighlightItems(vector<pair<unique_ptr<node>, int>>& multiselect, int StartX)
{

}
*/


void switchPanel(node*& currentSelected, node* panelFiles, int& modifiableY, int startX) {
    clearSelectionHighlight(currentSelected, startX, modifiableY);
    currentSelected = panelFiles;
    modifiableY = 93;
    highlightItem(currentSelected, 649 - startX, modifiableY);
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




bool compareByName(const node* a, const node* b, bool sorted) {
    std::string nameA = a->name;
    std::string nameB = b->name;

    // Convertim ambele șiruri la litere mici înainte de comparare
    std::transform(nameA.begin(), nameA.end(), nameA.begin(), [](unsigned char c) { return std::tolower(c); });
    std::transform(nameB.begin(), nameB.end(), nameB.begin(), [](unsigned char c) { return std::tolower(c); });

    return sorted ? (nameA < nameB) : (nameA > nameB);
}

bool compareByExtension(const node* a, const node* b, bool sorted) {
    std::string extA = a->extension;
    std::string extB = b->extension;

    // Convertim ambele extensii la litere mici înainte de comparare
    std::transform(extA.begin(), extA.end(), extA.begin(), [](unsigned char c) { return std::tolower(c); });
    std::transform(extB.begin(), extB.end(), extB.begin(), [](unsigned char c) { return std::tolower(c); });

    if (extA == extB) {
        return compareByName(a, b, sorted);
    }
    return sorted ? (extA < extB) : (extA > extB);
}

bool compareBySize(const node* a, const node* b, bool sorted) {
    if (a->size_file == b->size_file) {
        return compareByName(a, b, sorted);
    }
    return sorted ? (a->size_file < b->size_file) : (a->size_file > b->size_file);
}

bool compareByDate(const node* a, const node* b, bool sorted) {
    std::istringstream streamA(a->modified_date);
    std::istringstream streamB(b->modified_date);

    std::tm timeA = {}, timeB = {};

    streamA >> std::get_time(&timeA, "%m/%d/%Y");
    streamB >> std::get_time(&timeB, "%m/%d/%Y");

    auto timePointA = std::chrono::system_clock::from_time_t(std::mktime(&timeA));
    auto timePointB = std::chrono::system_clock::from_time_t(std::mktime(&timeB));

    if (sorted) {
        return timePointA < timePointB;
    }
    else {
        return timePointA > timePointB;
    }
}



void sort_by(const std::string& what_criteria, node*& panelnodes, bool sorted) {
    if (panelnodes == nullptr || panelnodes->next == nullptr) {
        // Lista este goală sau are doar un element
        return;
    }

    bool isBaseDir = panelnodes->name == "..";

    std::vector<node*> nodeList;
    node* current = panelnodes;

    // Adăugăm primul nod (dacă este "..") în vectorul sortării
    if (!isBaseDir) {
        nodeList.push_back(current);
        current = current->next;
    }
    else {
        current = current->next; // Trecem peste nodul ".." la sortare
    }

    while (current != nullptr) {
        nodeList.push_back(current);
        current = current->next;
    }

    // Sortarea vectorului de noduri în funcție de criteriul specificat
    if (what_criteria == "Name") {
        std::sort(nodeList.begin(), nodeList.end(), [&sorted](const node* a, const node* b) {
            return compareByName(a, b, sorted);
            });
    }
    else if (what_criteria == "Ext") {
        std::sort(nodeList.begin(), nodeList.end(), [&sorted](const node* a, const node* b) {
            return compareByExtension(a, b, sorted);
            });
    }
    else if (what_criteria == "Size") {
        std::sort(nodeList.begin(), nodeList.end(), [&sorted](const node* a, const node* b) {
            return compareBySize(a, b, sorted);
            });
    }
    else if (what_criteria == "Date") {
        std::sort(nodeList.begin(), nodeList.end(), [&sorted](const node* a, const node* b) {
            return compareByDate(a, b, sorted);
            });
    }


    // Reconstruim lista înlănțuită în funcție de ordinea sortată a nodurilor din vector
    if (isBaseDir)
    {
        panelnodes->next = nullptr;
        panelnodes->next = nodeList.front();
    }
    else
    {
        panelnodes = nullptr;
        panelnodes = nodeList.front(); // Noul început al listei
    }


    for (size_t i = 0; i < nodeList.size() - 1; ++i) {
        nodeList[i]->next = nodeList[i + 1];
        nodeList[i + 1]->prev = nodeList[i];
    }

    nodeList.back()->next = nullptr; // Închidem lista
}




std::vector<HWND> openPopupWindows; // Vector pentru a ține evidența ferestrelor pop-up deschise

HWND hwndEdit;
HWND hwndActionText;
HWND hwndOkButton;
HWND hwndCancelButton;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        hwndActionText = CreateWindowA(
            "STATIC",
            "",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 20, 260, 20, // Positioned above the text box
            hwnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        hwndEdit = CreateWindowA(
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
            20, 50, 260, 30, // Positioned below the action text
            hwnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        hwndOkButton = CreateWindowA(
            "BUTTON",
            "OK",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 90, 80, 25, // Positioned below the text box
            hwnd,
            (HMENU)1,
            GetModuleHandle(NULL),
            NULL
        );

        hwndCancelButton = CreateWindowA(
            "BUTTON",
            "Cancel",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            170, 90, 80, 25, // Positioned below the text box
            hwnd,
            (HMENU)2,
            GetModuleHandle(NULL),
            NULL
        );

        SetWindowTextA(hwndActionText, "Enter Text:");
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            char buffer[256] = { '\0' };
            GetWindowTextA(hwndEdit, buffer, 256);
            std::string* userInputPtr = reinterpret_cast<std::string*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            *userInputPtr = buffer;
            DestroyWindow(hwnd);
            return 0;
        }
        else if (LOWORD(wParam) == 2) // cancel
        {
            DestroyWindow(hwnd);
            return 0;
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        openPopupWindows.erase(std::remove(openPopupWindows.begin(), openPopupWindows.end(), hwnd), openPopupWindows.end());
        if (openPopupWindows.empty()) {
            UnregisterClassA("PopupWindowClass", GetModuleHandle(NULL)); // Elimină înregistrarea clasei dacă nu mai sunt ferestre
            PostQuitMessage(0); // Închide aplicația când nu mai sunt ferestre deschise
        }
        return 0;

    case WM_SETTEXT:
        SetWindowTextA(hwndActionText, reinterpret_cast<LPCSTR>(lParam));
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL open_popup_window(const std::string& action, std::string& user_input)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "PopupWindowClass";

    if (!GetClassInfoA(hInstance, "PopupWindowClass", &wc))
    {
        if (!RegisterClassA(&wc))
        {
            MessageBoxA(NULL, "Window Registration Failed!", "Error", MB_ICONERROR | MB_OK);
            return FALSE;
        }
    }

    HWND hwnd = CreateWindowA(
        "PopupWindowClass",
        "", // Numele ferestrei
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 180,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error", MB_ICONERROR | MB_OK);
        DWORD dwError = GetLastError();
        // Folosiți dwError pentru a identifica codul specific al erorii
        // MessageBoxA cu GetLastError() poate fi utilizat pentru a afișa codul de eroare
        return FALSE;
    }

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&user_input));
    SendMessageA(hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(action.c_str()));

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    openPopupWindows.push_back(hwnd); // Adăugați fereastra curentă în vector

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        if (msg.message == WM_QUIT)
        {
            break;
        }
    }

    return TRUE;
}

BOOL popup_error_window(const std::string& action) {
    LPCSTR message = action.c_str();

    int msgboxID = MessageBox(
        NULL,
        message,
        "Error",
        MB_ICONERROR | MB_OK | MB_DEFBUTTON1 | MB_TOPMOST // Opțiuni pentru fereastra de dialog
    );

    return (msgboxID == IDOK); // Poți trata rezultatul dacă este necesar
}


void popup_verify_window(const std::string& action) {
    LPCSTR message = action.c_str();
    MessageBox(NULL, message, "Verification Window", MB_OK | MB_ICONINFORMATION);

}


//function keys implementation
void Rename_File(node*& selectednode, const string& new_name, const string& given_path, bool& ok) {
    string old_file_name = given_path + selectednode->name + "." + selectednode->extension;
    string new_file_name = given_path + new_name + "." + selectednode->extension;

    if (MoveFile(old_file_name.c_str(), new_file_name.c_str())) {
        // Update the node's name if the file rename was successful
        selectednode->name = new_name;
        cout << "File renamed successfully!" << endl;
        ok = true;
    }
    else {

    }
}


void Rename_Directory(node*& selectednode, const std::string& new_name, const std::string& given_path, bool& ok) {
    std::string old_dir_name = given_path + selectednode->name;
    std::string new_dir_name = given_path + new_name;

    try {
        if (std::filesystem::exists(old_dir_name) && !std::filesystem::exists(new_dir_name)) {
            std::filesystem::rename(old_dir_name, new_dir_name);

            selectednode->name = new_name;
            ok = true;
            std::cout << "Directory renamed successfully!" << std::endl;
        }
        else {
            popup_error_window("Error renaming directory!");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


void create_directory(node*& panelnode, const std::string& name, const std::string& panel_path, bool& ok) {
    if (CreateDirectory((panel_path + name).c_str(), NULL)) {
        if (ERROR_ALREADY_EXISTS == GetLastError()) {
            popup_error_window("Folder already exists!");
            return;
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
            ok = true;
        }
        else {
            std::cout << "Failed to get last write time for the folder." << std::endl;
        }
    }
    else {
        popup_error_window("Failed to create folder.");
    }
}

void copyFile(const std::string& path, node*& selectednode, const std::string& destinationFolder, bool& ok) {
    std::string sourceFileName = selectednode->name + "." + selectednode->extension;
    std::string sourceFilePath = path + sourceFileName;
    std::string destinationPath;
    if (destinationFolder.back() == '\\')
        destinationPath = destinationFolder + sourceFileName;
    else
        destinationPath = destinationFolder + '\\' + sourceFileName;

    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(sourceFilePath, destinationPath, options);

        std::filesystem::file_time_type sourceTime = std::filesystem::last_write_time(sourceFilePath);
        std::filesystem::last_write_time(destinationPath, sourceTime);
        ok = true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        popup_error_window("Failed to copy file!");
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

void copyFile(const std::string& path, unique_ptr<node>& copiednode, const std::string& copied_path)
{
    std::string where_to_copy_path = path + copiednode->name + "." + copiednode -> extension;
    std::string old_path = copied_path + copiednode->name + "." + copiednode -> extension;



    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(old_path, where_to_copy_path, options);


    }
    catch (const std::filesystem::filesystem_error& e) {
        popup_error_window("Failed to copy folder!");
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }

}

void copyDirectory(const std::string& path, node*& selectednode, const std::string& destinationFolder, bool& ok) {
    std::string sourceFolderPath = path + selectednode->name; // Assuming selectednode->name is the folder name
    std::string destinationPath;

    if (destinationFolder.substr(destinationFolder.length() - 2) == "\\")
        destinationPath = destinationFolder + selectednode->name;
    else
        destinationPath = destinationFolder + "\\" + selectednode->name;

    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(sourceFolderPath, destinationPath, options);
        ok = true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        popup_error_window("Failed to copy folder!");
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

void copyDirectory(const std::string& path, unique_ptr<node>& copiednode, const std::string& copied_path)
{
    std::string where_to_copy_path = path + copiednode->name; 
    std::string old_path = copied_path + copiednode->name;

   

    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(old_path, where_to_copy_path, options);
       
    }
    catch (const std::filesystem::filesystem_error& e) {
        popup_error_window("Failed to copy folder!");
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }

}

void deleteFile(const string& basePath, node* selectednode, node*& panelnodes, bool& ok) {
    std::string filePath = basePath + selectednode->name + "." + selectednode->extension;
    if (DeleteFileA(filePath.c_str()) != 0) {
        ok = true;
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
        popup_error_window("Failed to delete file!");
    }
}


void deleteDirectory(const std::string& basePath, node* selectednode, node*& panelnodes, bool& ok) {
    std::string directoryPath = basePath + selectednode->name;
    if (RemoveDirectoryA(directoryPath.c_str()) != 0) {

        ok = true;
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
        popup_error_window("Failed to delete directory!");
    }
}

void moveFile(const std::string& basePath, node* selectednode, node*& panelnodes, const std::string& destinationFolder, bool& ok) {
    copyFile(basePath, selectednode, destinationFolder, ok); // Copy file to destination
    deleteFile(basePath, selectednode, panelnodes, ok); // Delete original file
    ok = true;
}

void moveDirectory(const std::string& basePath, node* selectednode, node*& panelnodes, const std::string& destinationFolder, bool& ok) {
    copyDirectory(basePath, selectednode, destinationFolder, ok); // Copy directory to destination
    deleteDirectory(basePath, selectednode, panelnodes, ok); // Delete original directory
    ok = true;
}


int main()
{
    initwindow(1290, 730, "Total Commander");
    analysis::Malware antivirus;
    node* leftPanelFiles = getFileList(left_panel_path);
    node* rightPanelFiles = getFileList(right_panel_path);
    node* mod_leftPanelFiles = new node;
    mod_leftPanelFiles = leftPanelFiles;
    node* mod_rightPanelFiles = new node;
    mod_rightPanelFiles = rightPanelFiles;
    currentSelectedLeft = leftPanelFiles;
    currentSelectedRight = rightPanelFiles;

    int maxim1 = -1; // pentru a ramane constant numarul de fisiere in stanga
    int maxim2 = -1; // pentru a ramane constant numarul de fisiere in dreapta
    int startY = 93;
    bool muzica_ok = false;
    int modifiable_left_Y = startY;
    int modifiable_right_Y = startY;
    bool modificat_stanga = false, modificat_dreapta = false;
    string pathMusicFile = "";
    double ysus1 = 98, ysus2 = 98;
    double yjos1 = 0, yjos2 = 0;
    int sort1 = 1, sort2 = 1;
    int page = 0;
    unique_ptr<node>coppiednode = make_unique<node>();
    vector<pair<unique_ptr<node>, int>>multiselect;
    bool is_coppied = false;
    string copied_path = "";
    


    while (true)
    {

        setactivepage(1 - page);
        cleardevice();

        draw_zones();

        //daca este in redare o melodie
        if (!muzica_ok)
            DrawShortcutButton(0, 660, 615, 690, "No music is playing");
        else
        {
            DrawShortcutButton(0, 660, 615, 690, pathMusicFile);
            DrawButton(615, 660, 1255, 690, "Click here to stop the music");
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


        if (modificat_stanga)
        {
            ysus1 = 98, yjos1 = 0;
            clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
            Free_Memory(leftPanelFiles);
            mod_leftPanelFiles = nullptr;
            currentSelectedLeft = nullptr;
            modifiable_left_Y = startY;
            //Free_Memory(mod_leftPanelFiles);
            //Free_Memory(currentSelectedLeft);
            leftPanelFiles = getFileList(left_panel_path);
            mod_leftPanelFiles = leftPanelFiles;
            currentSelectedLeft = leftPanelFiles;
            modificat_stanga = false;
        }
        if (modificat_dreapta)
        {

            ysus2 = 98, yjos2 = 0;
            clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
            Free_Memory(rightPanelFiles);
            // Free_Memory(mod_rightPanelFiles);
             //Free_Memory(currentSelectedRight);
            mod_rightPanelFiles = nullptr;
            currentSelectedRight = nullptr;
            modifiable_right_Y = startY;
            rightPanelFiles = getFileList(right_panel_path);
            mod_rightPanelFiles = rightPanelFiles;
            currentSelectedRight = rightPanelFiles;
            modificat_dreapta = false;
        }


        if (mod_leftPanelFiles != nullptr)
        {
            // Display file details in the WinBGIm window
            printFileDetailsLeftPanel(mod_leftPanelFiles, 0, startY); // Modify X position as needed
        }
        else
        {
            // Handle accordingly if file lists are not retrieved
            std::cerr << "Error retrieving file lists.\n";
        }

        if (mod_rightPanelFiles != nullptr)
        {

            printFileDetailsRightPanel(mod_rightPanelFiles, 650, startY); // Modify X position as needed
        }
        else
        {
            std::cerr << "Error retrieving file lists.\n";
        }

        if (which_panel)
        {
            multiselect.clear();
            multiselect.push_back(make_pair(make_unique<node>(*currentSelectedLeft), modifiable_left_Y));
            highlightItem(currentSelectedLeft, 4, modifiable_left_Y);  // Evidențiază elementul curent selectat
        }
        else
        {
        multiselect.clear();
        multiselect.push_back(make_pair(make_unique<node>(*currentSelectedRight), modifiable_right_Y));
        highlightItem(currentSelectedRight, 645, modifiable_right_Y); // Evidențiază elementul curent selectat
        }
            
        

        //panelul selectat
        setcolor(COLOR(47, 61, 76));
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if (which_panel)
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

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            Free_Memory(leftPanelFiles);
            Free_Memory(rightPanelFiles);
            //Free_Memory(currentSelectedLeft);
            //Free_Memory(currentSelectedRight);
            //Free_Memory(mod_leftPanelFiles);
            //Free_Memory(mod_rightPanelFiles);
            multiselect.clear();
            delete coppiednode.release();
            copied_path.clear();
            openPopupWindows.clear();
            closegraph();
            exit(0);
        }

        if (GetAsyncKeyState(VK_TAB) & 0x8000) {
            
            if (which_panel) {
                which_panel = false;
                switchPanel(currentSelectedLeft, leftPanelFiles, modifiable_left_Y, 4);
              
               // multiselect.push_back(make_pair(currentSelectedLeft, modifiable_left_Y));
            }
            else {
                which_panel = true;
                switchPanel(currentSelectedRight, rightPanelFiles, modifiable_right_Y, 645);
                
              //  multiselect.clear();
                //multiselect.push_back(make_pair(currentSelectedRight, modifiable_right_Y));
            }
            Sleep(50);
        }



        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
           
            if (which_panel && currentSelectedLeft->prev)
            {
                if (modifiable_left_Y - 25 > 90)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y -= 25;
                    currentSelectedLeft = currentSelectedLeft->prev;
                    printFileDetailsLeftPanel(mod_leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    if (maxim1 > 21 && ysus1 - latim1 > 92 && mod_leftPanelFiles->prev && currentSelectedLeft->prev)
                        ysus1 -= latim1, mod_leftPanelFiles = mod_leftPanelFiles->prev, currentSelectedLeft = currentSelectedLeft->prev;
                }
            }
            else if (!which_panel && currentSelectedRight->prev)
            {
                if (modifiable_right_Y - 25 > 90)
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    modifiable_right_Y -= 25;
                    currentSelectedRight = currentSelectedRight->prev;
                    printFileDetailsRightPanel(mod_rightPanelFiles, 645, startY);
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                }
                else
                {
                    if (maxim2 > 21 && ysus2 - latim2 > 92 && mod_rightPanelFiles->prev && currentSelectedRight->prev)
                        ysus2 -= latim2, mod_rightPanelFiles = mod_rightPanelFiles->prev, currentSelectedRight = currentSelectedRight->prev;
                }
            }
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
        
            if (which_panel && currentSelectedLeft->next)
            {
                if (modifiable_left_Y + 25 < 610)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y += 25;
                    currentSelectedLeft = currentSelectedLeft->next;
                    printFileDetailsLeftPanel(mod_leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    if (maxim1 > 21 && ysus1 + 21 * latim1 < 630 && mod_leftPanelFiles->next && currentSelectedLeft->next)
                        ysus1 += latim1, mod_leftPanelFiles = mod_leftPanelFiles->next, currentSelectedLeft = currentSelectedLeft->next;
                }
            }
            else if (!which_panel && currentSelectedRight->next)
            {
                if (modifiable_right_Y + 25 < 610)
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    modifiable_right_Y += 25;
                    currentSelectedRight = currentSelectedRight->next;
                    printFileDetailsRightPanel(mod_rightPanelFiles, 645, startY);
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                }
                else
                {
                    if (maxim2 > 21 && ysus2 + 21 * latim2 < 630 && mod_rightPanelFiles->next && currentSelectedRight->next)
                        ysus2 += latim2, mod_rightPanelFiles = mod_rightPanelFiles->next, currentSelectedRight = currentSelectedRight->next;
                }
            }
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
           
            if (muzica_ok)
            {
                if (GetAsyncKeyState(0x50) & 0x8000) // P letter
                {
                    StopMusic();
                    muzica_ok = false;
                }
                else // muzica oprita
                {
                    muzica_ok = true;
                    PlayMusic(pathMusicFile.c_str());
                }
            }
            Sleep(50);
        }

        //Ctrl + C
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('C') & 0x8000))
        {
            is_coppied = true;

            if (which_panel)
            {
                coppiednode = std::make_unique<node>(*currentSelectedLeft);
                copied_path = left_panel_path;
            }
            else
            {
                coppiednode = std::make_unique<node>(*currentSelectedRight);
                copied_path = right_panel_path;
            }

            Sleep(50);
        }

        //CTRL+V
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('V') & 0x8000))
        {
            if (which_panel)
            {
                if (is_coppied) {
                    if (coppiednode->is_directory)
                    {
                        copyDirectory(left_panel_path, coppiednode, copied_path);

                    }
                    else
                    {
                        copyFile(left_panel_path, coppiednode, copied_path);

                    }
                    modificat_stanga = true;
                }
            }
            else
            {
                if (is_coppied) {
                    if (coppiednode->is_directory)
                    {
                        copyDirectory(right_panel_path, coppiednode, copied_path);
                    }
                    else
                    {
                        copyFile(right_panel_path, coppiednode, copied_path);

                    }
                    modificat_dreapta = true;
                }
                }
            Sleep(50);
        }

        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('Z') & 0x8000))
        {
            is_coppied = false;
            if (which_panel)
                modificat_stanga = true;
            else
                modificat_dreapta = true;
            Sleep(15);
        }



        if (ismouseclick(WM_LBUTTONDOWN))
        {
            int mouseX = 0, mouseY = 0;
            getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

            //scrollbar
            if (isPointInInterval(mouseX, mouseY, 615, 60, 640, 90))
                if (maxim1 > 21 && ysus1 - latim1 > 92 && mod_leftPanelFiles->prev && currentSelectedLeft->prev)
                    ysus1 -= latim1, mod_leftPanelFiles = mod_leftPanelFiles->prev, currentSelectedLeft = currentSelectedLeft->prev;
            if (isPointInInterval(mouseX, mouseY, 615, 630, 640, 660))
                if (maxim1 > 21 && ysus1 + 21 * latim1 < 627 && leftPanelFiles->next && currentSelectedLeft->next)
                    ysus1 += latim1, mod_leftPanelFiles = mod_leftPanelFiles->next, currentSelectedLeft = currentSelectedLeft->next;

            if (isPointInInterval(mouseX, mouseY, 1255, 60, 1280, 90))
                if (maxim2 > 21 && ysus2 - latim2 > 92 && mod_rightPanelFiles->prev && currentSelectedRight->prev)
                    ysus2 -= latim2, mod_rightPanelFiles = mod_rightPanelFiles->prev, currentSelectedRight = currentSelectedRight->prev;
            if (isPointInInterval(mouseX, mouseY, 1255, 630, 1280, 660))
                if (maxim2 > 21 && ysus2 + 21 * latim2 < 627 && mod_rightPanelFiles->next && currentSelectedRight->next)
                    ysus2 += latim2, mod_rightPanelFiles = mod_rightPanelFiles->next, currentSelectedRight = currentSelectedRight->next;

            // lupe
            if (isPointInInterval(mouseX, mouseY, 615, 30, 640, 60))
            {
                
                if (!which_panel) which_panel = true;
                // lupa stanga
                string destinationFolder = "";
                open_popup_window("Where to:", destinationFolder);
                if (!destinationFolder.empty()) {
                    if (!pathExists(destinationFolder))
                    {
                        popup_error_window("This path is not valid!");
                    }
                    else
                    {
                        if (destinationFolder.back() != '\\')
                            destinationFolder += '\\';
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        currentSelectedLeft = mod_leftPanelFiles = nullptr;
                        AccessFolder(left_panel_path, destinationFolder, leftPanelFiles);
                        currentSelectedLeft = leftPanelFiles;
                        mod_leftPanelFiles = leftPanelFiles;
                        modifiable_left_Y = startY;
                        highlightItem(mod_leftPanelFiles, 4, modifiable_left_Y);
                        modificat_stanga = false;
                        maxim1 = -1;
                        ysus1 = 98;
                        yjos1 = 0;

                    }
                    destinationFolder.clear();
                }
                else
                    continue;
                Sleep(50);
            }

            if (isPointInInterval(mouseX, mouseY, 1255, 30, 1280, 60))
            {
                
                if (which_panel)  which_panel = false;
                // lupa dreapta
                string destinationFolder = "";
                open_popup_window("Where to:", destinationFolder);
                if (!destinationFolder.empty()) {
                    if (!pathExists(destinationFolder))
                    {
                        popup_error_window("This path is not valid!");
                    }
                    else
                    {
                        if (destinationFolder.back() != '\\')
                            destinationFolder += '\\';
                        clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                        AccessFolder(right_panel_path, destinationFolder, rightPanelFiles);
                        currentSelectedRight = rightPanelFiles;
                        mod_rightPanelFiles = rightPanelFiles;
                        modifiable_right_Y = startY;
                        highlightItem(mod_rightPanelFiles, 646, modifiable_right_Y);
                        modificat_dreapta = false;
                        maxim2 = -1;
                        ysus2 = 98;
                        yjos2 = 0;

                    }
                    destinationFolder.clear();
                }
                else
                    continue;

                Sleep(50);
            }

            //butonul muzical
            if (isPointInInterval(mouseX, mouseY, 800, 663, 1140, 687) && muzica_ok)
                muzica_ok = false;
            if (isPointInInterval(mouseX, mouseY, 615, 660, 640, 690))
            {

                open_popup_window("Where to:", pathMusicFile);
                if (!pathMusicFile.empty())
                {

                    if (!pathMusicExists(pathMusicFile))
                    {
                        popup_error_window("Invalid music Path!");
                    }
                    else
                        if (!muzica_ok)
                        {
                            PlayMusic(pathMusicFile.c_str());
                            muzica_ok = true;
                        }
                        else
                        {
                            StopMusic();
                            PlayMusic(pathMusicFile.c_str());
                        }
                }
                else
                    continue;
                Sleep(50);
            }



            //comenzile rapide
            if (isPointInInterval(mouseX, mouseY, 0, 690, 252, 720))
            { //F2
                string new_name = "";
                open_popup_window("Rename to:", new_name);
                if (!new_name.empty()) {
                    bool ok = false;
                    if (which_panel)
                    {
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        if (!currentSelectedLeft->is_directory)
                        {
                            Rename_File(currentSelectedLeft, new_name, left_panel_path, ok);
                            if (ok) popup_verify_window("File renamed succesfully!");
                        }
                        else
                        {
                            Rename_Directory(currentSelectedLeft, new_name, left_panel_path, ok);
                            if (ok) popup_verify_window("Directory renamed succesfully!");
                        }
                        // sort_by("Name", leftPanelFiles);
                         //sort_by("Id", leftPanelFiles);
                        modificat_stanga = true;
                        modifiable_left_Y = startY;
                        maxim1 = -1;
                        ysus1 = 98;
                        yjos1 = 0;
                    }
                    else
                    {
                        clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                        if (!currentSelectedRight->is_directory)
                        {
                            Rename_File(currentSelectedRight, new_name, right_panel_path, ok);
                            if (ok) popup_verify_window("File renamed successfully!");
                        }
                        else
                        {
                            Rename_Directory(currentSelectedRight, new_name, right_panel_path, ok);
                            if (ok) popup_verify_window("Directory renamed successfully!");
                        }
                        // sort_by("Name", rightPanelFiles);
                         //sort_by("Id", rightPanelFiles);
                        modificat_dreapta = true;
                        modifiable_right_Y = startY;
                        maxim2 = -1;
                        ysus2 = 98;
                        yjos2 = 0;
                    }

                    new_name.clear();

                }
                else
                    continue;
                Sleep(50);
            }

            if (isPointInInterval(mouseX, mouseY, 253, 690, 504, 720))
            { //F3
                string new_name = "";
                open_popup_window("Name the directory to:", new_name);
                if (!new_name.empty()) {
                    bool ok = false;
                    if (which_panel)
                    {
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        create_directory(leftPanelFiles, new_name, left_panel_path, ok);
                        if (ok) popup_verify_window("Directory created successfully!");
                        // sort_by("Name", leftPanelFiles);
                         //sort_by("Id", leftPanelFiles);
                        modificat_stanga = true;
                        modifiable_left_Y = startY;
                        maxim1 = -1;
                        ysus1 = 98;
                        yjos1 = 0;
                    }
                    else
                    {
                        clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                        create_directory(rightPanelFiles, new_name, right_panel_path, ok);
                        if (ok) popup_verify_window("Directory created successfully!");
                        // sort_by("Name", rightPanelFiles);
                         //sort_by("Id", rightPanelFiles);
                        modificat_dreapta = true;
                        modifiable_right_Y = startY;
                        maxim2 = -1;
                        ysus2 = 98;
                        yjos2 = 0;
                    }
                    new_name.clear();
                }
                else continue;
                Sleep(50);
            }
            if (isPointInInterval(mouseX, mouseY, 505, 690, 756, 720))
            { //F4
                string destinationfolder = "";
                open_popup_window("Where to:", destinationfolder);
                if (!destinationfolder.empty()) {
                    bool ok = false;
                    if (which_panel)
                    {

                        if (!currentSelectedLeft->is_directory)
                        {
                            copyFile(left_panel_path, currentSelectedLeft, destinationfolder, ok);
                            if (ok) popup_verify_window("File copied successfully!");
                        }
                        else
                        {
                            copyDirectory(left_panel_path, currentSelectedLeft, destinationfolder, ok);
                            if (ok) popup_verify_window("Directory copied successfully!");
                        }
                    }
                    else
                    {
                        if (!currentSelectedRight->is_directory)
                        {
                            copyFile(right_panel_path, currentSelectedRight, destinationfolder, ok);
                            if (ok) popup_verify_window("File copied successfully!");
                        }
                        else
                        {
                            copyDirectory(right_panel_path, currentSelectedRight, destinationfolder, ok);
                            if (ok) popup_verify_window("Directory copied successfully!");
                        }
                    }
                    destinationfolder.clear();
                }
                else
                    continue;
                Sleep(50);
            }

            if (isPointInInterval(mouseX, mouseY, 1010, 690, 1280, 720))
            {
                //F5
                bool ok = false;
                if (which_panel)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    if (!currentSelectedLeft->is_directory)
                    {
                        deleteFile(left_panel_path, currentSelectedLeft, leftPanelFiles, ok);
                        popup_verify_window("File deleted successfully!");
                    }
                    else
                        if (currentSelectedLeft->name == "..")
                            popup_verify_window("Failed to delete the directory!");
                        else
                        {
                            deleteDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, ok);
                            popup_verify_window("Directory deleted successfully!");
                        }
                    modifiable_left_Y = startY;
                    modificat_stanga = true;
                    maxim1 = -1;
                    ysus1 = 98;
                    yjos1 = 0;
                }
                else
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    if (!currentSelectedRight->is_directory)
                    {
                        deleteFile(right_panel_path, currentSelectedRight, rightPanelFiles, ok);
                        popup_verify_window("File deleted successfully!");
                    }
                    else
                    {
                        deleteDirectory(right_panel_path, currentSelectedRight, rightPanelFiles, ok);
                        popup_verify_window("Directory deleted successfully!");
                    }
                    modifiable_right_Y = startY;
                    modificat_dreapta = true;
                    maxim2 = -1;
                    ysus2 = 98;
                    yjos2 = 0;
                }
                Sleep(50);
            }


            if (isPointInInterval(mouseX, mouseY, 1009, 690, 1280, 720))
            { //F6
                string destinationFolder = "";
                open_popup_window("Move to:", destinationFolder);
                if (!destinationFolder.empty()) {
                    bool ok = false;
                    if (which_panel)
                    {
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        if (!currentSelectedLeft->is_directory)
                        {
                            moveFile(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder, ok);
                            if (ok) popup_verify_window("File moved succesfully!");
                        }
                        else
                        {
                            moveDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder, ok);
                            if (ok) popup_verify_window("File moved succesfully!");
                        }
                        modifiable_left_Y = startY;
                        modificat_stanga = true;
                        maxim1 = -1;
                        ysus1 = 98;
                        yjos1 = 0;
                    }
                    else
                    {
                        clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                        if (!currentSelectedRight->is_directory)
                        {
                            moveFile(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder, ok);
                            if (ok) popup_verify_window("File moved succesfully!");
                        }
                        else
                        {
                            moveDirectory(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder, ok);
                            if (ok) popup_verify_window("File moved succesfully!");
                        }
                        modifiable_right_Y = startY;
                        modificat_dreapta = true;
                        maxim2 = -1;
                        ysus2 = 98;
                        yjos2 = 0;
                    }
                    destinationFolder.clear();
                }
                else
                    continue;
                Sleep(50);
            }

            //sortare panel stanga
            if (isPointInInterval(mouseX, mouseY, 0, 60, 270, 90))
            {
                which_panel = true;
                if (sort1 == 1)
                {
                    sort1 = 2;

                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    sort_by("Name", leftPanelFiles, false);
                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;
                    modifiable_left_Y = startY;
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare descresc dupa name
                }

                else
                {
                    sort1 = 1;

                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Name", leftPanelFiles, true);
                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;

                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare cresc dupa name
                }
            }
            if (isPointInInterval(mouseX, mouseY, 270, 60, 355, 90))
            {
                which_panel = true;
                if (sort1 == 3)
                {
                    sort1 = 4;

                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Ext", leftPanelFiles, false);
                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;

                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare descresc extensie
                }

                else
                {
                    sort1 = 3;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Ext", leftPanelFiles, true);

                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare cresc extensie
                }
            }
            if (isPointInInterval(mouseX, mouseY, 355, 60, 440, 90))
            {
                which_panel = true;
                if (sort1 == 5)
                {
                    sort1 = 6;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Size", leftPanelFiles, false);

                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;

                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare descresc size
                }

                else
                {
                    sort1 = 5;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Size", leftPanelFiles, true);

                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;

                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare cresc size
                }
            }
            if (isPointInInterval(mouseX, mouseY, 440, 60, 615, 90))
            {
                which_panel = true;
                if (sort1 == 7)
                {
                    sort1 = 8;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Date", leftPanelFiles, false);

                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;

                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare descresc data
                }

                else
                {
                    sort1 = 7;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    currentSelectedLeft = nullptr;
                    mod_leftPanelFiles = nullptr;
                    modifiable_left_Y = startY;
                    sort_by("Date", leftPanelFiles, true);
                    currentSelectedLeft = leftPanelFiles;
                    mod_leftPanelFiles = leftPanelFiles;
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                    ysus1 = 98;
                    yjos1 = 0;
                    //sortare cresc data
                }
            }

            //sortare panel dreapta
            if (isPointInInterval(mouseX, mouseY, 660, 60, 910, 90))
            {
                which_panel = false;
                if (sort2 == 1)
                {
                    sort2 = 2;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Name", rightPanelFiles, false);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare descresc dupa name
                }

                else
                {
                    sort2 = 1;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Name", rightPanelFiles, true);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare cresc dupa name
                }
            }
            if (isPointInInterval(mouseX, mouseY, 910, 60, 995, 90))
            {
                which_panel = false;
                if (sort2 == 3)
                {
                    sort2 = 4;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Ext", rightPanelFiles, false);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare descresc extensie
                }

                else
                {
                    sort2 = 3;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Ext", rightPanelFiles, true);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare cresc extensie
                }
            }
            if (isPointInInterval(mouseX, mouseY, 995, 60, 1080, 90))
            {
                which_panel = false;
                if (sort2 == 5)
                {
                    sort2 = 6;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Size", rightPanelFiles, false);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare descresc size
                }

                else
                {
                    sort2 = 5;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Size", rightPanelFiles, true);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare cresc size
                }
            }
            if (isPointInInterval(mouseX, mouseY, 1080, 60, 1255, 90))
            {
                which_panel = false;
                if (sort2 == 7)
                {
                    sort2 = 8;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Date", rightPanelFiles, false);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare descresc data
                }

                else
                {
                    sort2 = 7;
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    mod_rightPanelFiles = nullptr;
                    currentSelectedRight = nullptr;
                    modifiable_right_Y = startY;
                    sort_by("Date", rightPanelFiles, true);
                    currentSelectedRight = rightPanelFiles;
                    mod_rightPanelFiles = rightPanelFiles;
                    highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                    ysus2 = 98;
                    yjos2 = 0;
                    //sortare cresc data
                }
            }
        }

        //afisare scrollbar
        if (maxim1 < 22)
            yjos1 = 627;
        else
            yjos1 = ysus1 + 21 * latim1;

        if (maxim2 < 22)
            yjos2 = 627;
        else
            yjos2 = ysus2 + 21 * latim2;
        setfillstyle(SOLID_FILL, COLOR(47, 61, 76));
        if (yjos1 > 627)
            yjos1 = 627;
        if (yjos2 > 627)
            yjos2 = 627;
        bar(620, ysus1, 637, yjos1);
        bar(1260, ysus2, 1277, yjos2);

        //afisare sageti sortare
        if (mousex() >= 0 && mousex() <= 615 && mousey() >= 60 && mousey() <= 90)
        {
            if (sort1 == 2)
                DrawArrow(257, 76, 267, 86, "v");
            else
                if (sort1 == 1)
                    DrawArrow(257, 80, 267, 90, "^");
                else
                    if (sort1 == 4)
                        DrawArrow(344, 76, 354, 86, "v");
                    else
                        if (sort1 == 3)
                            DrawArrow(344, 80, 354, 90, "^");
                        else
                            if (sort1 == 6)
                                DrawArrow(431, 77, 441, 86, "v");
                            else
                                if (sort1 == 5)
                                    DrawArrow(431, 80, 441, 90, "^");
                                else
                                    if (sort1 == 8)
                                        DrawArrow(603, 76, 613, 86, "v");
                                    else
                                        if (sort1 == 7)
                                            DrawArrow(603, 80, 613, 90, "^");
        }

        if (mousex() >= 640 && mousex() <= 1255 && mousey() >= 60 && mousey() <= 90)
        {
            if (sort2 == 2)
                DrawArrow(898, 76, 908, 86, "v");
            else
                if (sort2 == 1)
                    DrawArrow(898, 80, 908, 90, "^");
                else
                    if (sort2 == 4)
                        DrawArrow(985, 76, 995, 86, "v");
                    else
                        if (sort2 == 3)
                            DrawArrow(985, 80, 995, 90, "^");
                        else
                            if (sort2 == 6)
                                DrawArrow(1070, 77, 1080, 86, "v");
                            else
                                if (sort2 == 5)
                                    DrawArrow(1070, 80, 1080, 90, "^");
                                else
                                    if (sort2 == 8)
                                        DrawArrow(1243, 76, 1253, 86, "v");
                                    else
                                        if (sort2 == 7)
                                            DrawArrow(1243, 80, 1253, 90, "^");
        }


        if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            if (which_panel) {
                if (currentSelectedLeft != nullptr) {
                    if (currentSelectedLeft->is_directory) {
                        clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                        AccessFolder(left_panel_path, currentSelectedLeft, leftPanelFiles);
                        mod_leftPanelFiles = leftPanelFiles;
                        modifiable_left_Y = startY;
                        // highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                        modificat_stanga = false;
                        maxim1 = -1;
                        ysus1 = 98;
                        yjos1 = 0;
                    }
                    else {
                        OpenFile(left_panel_path, currentSelectedLeft);
                    }
                }
            }
            else {
                if (currentSelectedRight != nullptr) {
                    if (currentSelectedRight->is_directory) {
                        clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                        AccessFolder(right_panel_path, currentSelectedRight, rightPanelFiles);
                        mod_rightPanelFiles = rightPanelFiles;
                        modifiable_right_Y = startY;
                        highlightItem(currentSelectedRight, 645, modifiable_right_Y);
                        maxim2 = -1;
                        ysus2 = 98, yjos2 = 0;
                        modificat_dreapta = false;
                    }
                    else {
                        OpenFile(right_panel_path, currentSelectedRight);
                    }
                }
            }
            Sleep(50);
        }


        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {

            if (which_panel)
                if (antivirus.malware_Checking(currentSelectedLeft))
                    popup_error_window("This file contains a virus!");
                else
                    popup_verify_window("This file is safe to use!");
            else
                if (antivirus.malware_Checking(currentSelectedRight))
                    popup_error_window("This file contains a virus!");
                else
                    popup_verify_window("This file is safe to use!");
            Sleep(50);
        }


        if (GetAsyncKeyState(VK_F2) & 0x8000)
        {
            string new_name = "";
            open_popup_window("Rename to:", new_name);
            if (!new_name.empty()) {
                bool ok = false;
                if (which_panel)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    if (!currentSelectedLeft->is_directory)
                    {
                        Rename_File(currentSelectedLeft, new_name, left_panel_path, ok);
                        if (ok) popup_verify_window("File renamed succesfully!");
                    }
                    else
                    {
                        Rename_Directory(currentSelectedLeft, new_name, left_panel_path, ok);
                        if (ok) popup_verify_window("Directory renamed succesfully!");
                    }
                    // sort_by("Name", leftPanelFiles);
                     //sort_by("Id", leftPanelFiles);
                    modificat_stanga = true;
                    modifiable_left_Y = startY;
                    maxim1 = -1;
                    ysus1 = 98;
                    yjos1 = 0;
                }
                else
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    if (!currentSelectedRight->is_directory)
                    {
                        Rename_File(currentSelectedRight, new_name, right_panel_path, ok);
                        if (ok) popup_verify_window("File renamed successfully!");
                    }
                    else
                    {
                        Rename_Directory(currentSelectedRight, new_name, right_panel_path, ok);
                        if (ok) popup_verify_window("Directory renamed successfully!");
                    }
                    // sort_by("Name", rightPanelFiles);
                     //sort_by("Id", rightPanelFiles);
                    modificat_dreapta = true;
                    modifiable_right_Y = startY;
                    maxim2 = -1;
                    ysus2 = 98;
                    yjos2 = 0;
                }

                new_name.clear();

            }
            else
                continue;
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_F3) & 0x8000)
        {
            string new_name = "";
            open_popup_window("Name the directory to:", new_name);
            if (!new_name.empty()) {
                bool ok = false;
                if (which_panel)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    create_directory(leftPanelFiles, new_name, left_panel_path, ok);
                    if (ok) popup_verify_window("Directory created successfully!");
                    // sort_by("Name", leftPanelFiles);
                     //sort_by("Id", leftPanelFiles);
                    modificat_stanga = true;
                    modifiable_left_Y = startY;
                    maxim1 = -1;
                    ysus1 = 98;
                    yjos1 = 0;
                }
                else
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    create_directory(rightPanelFiles, new_name, right_panel_path, ok);
                    if (ok) popup_verify_window("Directory created successfully!");
                    // sort_by("Name", rightPanelFiles);
                     //sort_by("Id", rightPanelFiles);
                    modificat_dreapta = true;
                    modifiable_right_Y = startY;
                    maxim2 = -1;
                    ysus2 = 98;
                    yjos2 = 0;
                }
                new_name.clear();
            }
            else continue;
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_F4) & 0x8000)
        {
            string destinationfolder = "";
            open_popup_window("Where to:", destinationfolder);
            if (!destinationfolder.empty()) {
                bool ok = false;
                if (which_panel)
                {

                    if (!currentSelectedLeft->is_directory)
                    {
                        copyFile(left_panel_path, currentSelectedLeft, destinationfolder, ok);
                        if (ok) popup_verify_window("File copied successfully!");
                    }
                    else
                    {
                        copyDirectory(left_panel_path, currentSelectedLeft, destinationfolder, ok);
                        if (ok) popup_verify_window("Directory copied successfully!");
                    }
                }
                else
                {
                    if (!currentSelectedRight->is_directory)
                    {
                        copyFile(right_panel_path, currentSelectedRight, destinationfolder, ok);
                        if (ok) popup_verify_window("File copied successfully!");
                    }
                    else
                    {
                        copyDirectory(right_panel_path, currentSelectedRight, destinationfolder, ok);
                        if (ok) popup_verify_window("Directory copied successfully!");
                    }
                }
                destinationfolder.clear();
            }
            else
                continue;
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_F5) & 0x8000)
        {
            bool ok = false;
            if (which_panel)
            {

                clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                if (!currentSelectedLeft->is_directory)
                {
                    deleteFile(left_panel_path, currentSelectedLeft, leftPanelFiles, ok);
                    if (ok) popup_verify_window("File deleted successfully!");
                }
                else
                    if (currentSelectedLeft->name == "..")
                        popup_verify_window("Failed to delete the directory!");
                    else
                    {
                        deleteDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, ok);
                        if (ok) popup_verify_window("Directory deleted succesfully!");
                    }
                modifiable_left_Y = startY;
                modificat_stanga = true;
                maxim1 = -1;
                ysus1 = 98;
                yjos1 = 0;
            }
            else
            {
                clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                if (!currentSelectedRight->is_directory)
                {
                    deleteFile(right_panel_path, currentSelectedRight, rightPanelFiles, ok);
                    if (ok) popup_verify_window("File deleted succesfully!");
                }
                else
                    if (currentSelectedLeft->name == "..")
                        popup_verify_window("Failed to delete the directory!");
                    else
                    {
                        deleteDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, ok);
                        if (ok) popup_verify_window("Directory deleted succesfully!");
                    }
                modifiable_right_Y = startY;
                modificat_dreapta = true;
                maxim2 = -1;
                ysus2 = 98;
                yjos2 = 0;
            }
            Sleep(50);
        }

        if (GetAsyncKeyState(VK_F6) & 0x8000)
        {
            string destinationFolder = "";
            open_popup_window("Move to:", destinationFolder);
            if (!destinationFolder.empty()) {
                bool ok = false;
                if (which_panel)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    if (!currentSelectedLeft->is_directory)
                    {
                        moveFile(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder, ok);
                        if (ok) popup_verify_window("File moved succesfully!");
                    }
                    else
                    {
                        moveDirectory(left_panel_path, currentSelectedLeft, leftPanelFiles, destinationFolder, ok);
                        if (ok) popup_verify_window("File moved succesfully!");
                    }
                    modifiable_left_Y = startY;
                    modificat_stanga = true;
                    maxim1 = -1;
                    ysus1 = 98;
                    yjos1 = 0;
                }
                else
                {
                    clearSelectionHighlight(currentSelectedRight, 645, modifiable_right_Y);
                    if (!currentSelectedRight->is_directory)
                    {
                        moveFile(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder, ok);
                        if (ok) popup_verify_window("File moved succesfully!");
                    }
                    else
                    {
                        moveDirectory(right_panel_path, currentSelectedRight, rightPanelFiles, destinationFolder, ok);
                        if (ok) popup_verify_window("File moved succesfully!");
                    }
                    modifiable_right_Y = startY;
                    modificat_dreapta = true;
                    maxim2 = -1;
                    ysus2 = 98;
                    yjos2 = 0;
                }
                destinationFolder.clear();
            }
            else
                continue;
            Sleep(50);
        }

        //delay(50);
        Sleep(50);
        setvisualpage(1 - page);
        page = 1 - page;

    }

    closegraph();
    openPopupWindows.clear();
    delete coppiednode.release();
    multiselect.clear();
    Free_Memory(leftPanelFiles);
    Free_Memory(rightPanelFiles);
    return 0;
}
