#include <windows.h>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <winbgim.h>
#include <graphics.h>
#define Nr_of_Buttons 5
using namespace std;

char left_panel_path[FILENAME_MAX] = "C:\\";
char right_panel_path[FILENAME_MAX] = "D:\\";



struct node{

    string name;
    string extension;
    double size_file;
    string modified_date;
    bool is_directory;
    node* next;
};


bool pathExists(const char* path) {
    DWORD attributes = GetFileAttributes(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}


void display_path(int x1, int y1, int x2, int y2, const char* path) {
    settextstyle(4, 0, 12);
    setcolor(COLOR(151, 159, 165));
    setbkcolor(COLOR(47, 61, 76));

    if (pathExists(path)) {
        const char* lastFolder = path + strlen(path) - 2;
        while (lastFolder > path && *lastFolder != '\\') {
            lastFolder--;
        }

        if (*lastFolder == '\\') {
            lastFolder++; // Move past the '\\'
        }

        int textWidth = textwidth((char*)lastFolder);

        if (textWidth > x2 - x1) {
            int visibleLength = x2 - x1 - 4; // To leave space for ".."

            if (visibleLength > 0) {
                const char* truncatedPath = lastFolder + strlen(lastFolder) - visibleLength + 2;
                char displayPath[FILENAME_MAX];
                snprintf(displayPath, sizeof(displayPath), "..%s", truncatedPath);
                outtextxy(x1 + 2, y1 + 7, displayPath);
            }
        } else {
            outtextxy(x1 + 2, y1 + 7, (char*)path);
        }
    } else {
        std::cout << "Path is not valid or doesn't exist\n";
        // Handle accordingly, for example, displaying an error message
    }
}


void DrawArrow(int x1, int y1, int x2, int y2, char* buttonText)
{
   settextstyle(4,0,15);
   setcolor(COLOR(47,61,76));
   int textX = (x1 + x2) / 2 - textwidth(buttonText) / 2;
   int textY = (y1 + y2) / 2 - textheight(buttonText) / 2;
   settextstyle(4,0,12);
   setbkcolor(COLOR(255,255,255));
   outtextxy(textX, textY, buttonText);
}



void DrawShortcutButton(int x1, int y1, int x2, int y2, char* buttonText)
{
   settextstyle(4,0,12);
   setcolor(COLOR(151,159,165));
   int textX = (x1 + x2) / 2 - textwidth(buttonText) / 2;
   int textY = (y1 + y2) / 2 - textheight(buttonText) / 2;
   setbkcolor(COLOR(47,61,76));
   outtextxy(textX, textY, buttonText);
}

void DrawButton(int x1, int y1, int x2, int y2, char* buttonText)
{
   settextstyle(4,0,12);
   setcolor(COLOR(47,61,76));
   int textX = (x1 + x2) / 2 - textwidth(buttonText) / 2;
   int textY = (y1 + y2) / 2 - textheight(buttonText) / 2;
   settextstyle(4,0,12);
   setbkcolor(COLOR(151,159,165));
   outtextxy(textX, textY, buttonText);
}


void free_space(const char* current_path, int x1, int y1, int x2, int y2) {
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL success = GetDiskFreeSpaceEx(
        current_path,
        &freeBytesAvailable,
        &totalNumberOfBytes,
        &totalNumberOfFreeBytes
    );

    if (success) {
        char availableSpaceStr[100];
        snprintf(availableSpaceStr, sizeof(availableSpaceStr), "%llu GB free of %llu GB",
                 (freeBytesAvailable.QuadPart) / (1024ULL * 1024ULL * 1024ULL),
                 (totalNumberOfBytes.QuadPart) / (1024ULL * 1024ULL * 1024ULL));

        // Display text in the graphics window
        DrawShortcutButton(x1, y1, x2, y2, availableSpaceStr);
    } else {
        std::cerr << "Error getting disk space information.\n";
    }
}

void shortcut_buttons()
{
    // comenzi rapide
    DrawShortcutButton(0, 690, 252, 720, (char*) "F2 Rename");
    DrawShortcutButton(252, 690, 504, 720, (char*)"F3 Mkdir");
    DrawShortcutButton(504, 690, 756, 720, (char*)"F4 Copy");
    DrawShortcutButton(756, 690, 1008, 720, (char*)"F5 Delete");
    DrawShortcutButton(1008, 690, 1280, 720, (char*) "F6 Move");

    //liniile dintre butoanele comenzilor rapide de jos
    bar(252, 690, 257, 720);
    bar(509, 690, 514, 720);
    bar(766, 690, 771, 720);
    bar(1023, 690, 1028, 720);

}

void draw_first_panel()
{
    //sidebar de sus (display the memory)
    free_space(left_panel_path, 0, 0, 640, 30);

    //display the path
    display_path(0, 30, 640, 60, left_panel_path);

    //butoanele de deasupra fisierelor panel 1
    bar(270, 60, 273, 90);
    bar(355, 60, 358, 90);
    bar(440, 60, 443, 90);
 //denumiri panel 1
    DrawButton(0, 60, 279, 90, (char*) "Name");
    DrawButton(280, 60, 335, 90, (char*) "Ext");
    DrawButton(355, 60, 440, 90, (char*) "Size");
    DrawButton(440, 60, 615, 90, (char*) "Date");
    DrawArrow(624, 79, 640, 90, (char*) "^");
    DrawArrow(626, 647, 640, 660, (char*) "v");

     //liniile ce delimiteaza informatiile
    bar(270,90,273,630);
    bar(355, 90, 358, 630);
    bar(440, 90, 443, 630);

}

void draw_second_panel()
{
    //title bar de sus (display the memory)
    //free_space(right_panel_path, 640, 0, 1280, 30);

    //display the path
   // display_path(x1,y1,x2,y2, right_panel_path);

    // butoanele de deasupra fisierelor panel 2
     bar(910, 60, 913, 90);
    bar(995, 60, 998, 90);
    bar(1080, 60, 1083, 90);

  //denumiri panel 2
    DrawButton(660, 60, 910, 90, (char*) "Name");
    DrawButton(910, 60, 995, 90, (char*) "Ext");
    DrawButton(995, 60, 1080, 90, (char*) "Size");
    DrawButton(1080, 60, 1255, 90, (char*) "Date");

    //liniile ce delimiteaza informatiile
    bar(910, 90, 913, 630);
    bar(995, 90, 998, 630);
    bar(1080, 90, 1083, 630);
    DrawArrow(1264, 79, 1280, 90, (char*) "^");
    DrawArrow(1266, 647, 1280, 660, (char*) "v");
}

node* getFileList(const char* path) {
    if (!pathExists(path)) {
        cout << "Path is not valid or doesn't exist\n";
        return nullptr;
    }

    WIN32_FIND_DATA fileData;
    HANDLE hFind = FindFirstFile((string(path) + "\\*").c_str(), &fileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "Error finding the first file\n";
        return nullptr;
    }

    node* head = nullptr;
    node* current = nullptr;

    do {
        node* newNode = new node;
        newNode->name = fileData.cFileName;

        // Extract extension
        size_t pos = newNode->name.find_last_of('.');
        if (pos != string::npos) {
            newNode->extension = newNode->name.substr(pos + 1);
            newNode->name = newNode->name.substr(0, pos);
        } else {
            newNode->extension = "<DIR>";
        }

        // Check if it's a directory
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            newNode->size_file = -1; // Placeholder for directories
        } else {
            // Calculate size in megabytes for files
            newNode->size_file = static_cast<double>(fileData.nFileSizeLow) / (1024 * 1024);
        }

        // Convert FILETIME to local time string for modified date
        FILETIME lastWriteTime = fileData.ftLastWriteTime;
        SYSTEMTIME sysTime;
        FileTimeToSystemTime(&lastWriteTime, &sysTime);
        char modifiedDate[128] = {'\0'};
        GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, nullptr, modifiedDate, 128);
        newNode->modified_date = modifiedDate;

        newNode->is_directory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = current->next;
        }
    } while (FindNextFile(hFind, &fileData) != 0);

    FindClose(hFind);
    return head;
}


void printFileDetails(node* fileList, int startX, int startY) {
    int y = startY;
    settextstyle(4, 0, 12); // Set text style
    setcolor(COLOR(151, 159, 165)); // Set text color

    while (fileList != nullptr) {
        char fileInfo[512] = {'\0'};
        sprintf(fileInfo, "%s.%s %s %.2f MB %s", fileList->name.c_str(), fileList->extension.c_str(),
                fileList->modified_date.c_str(), fileList->size_file, fileList->is_directory ? "<DIR>" : "");

        outtextxy(startX, y, fileInfo); // Output file details

        y += textheight(fileInfo) + 15; // Increment Y position for the next file details
        fileList = fileList->next;
    }
}

void Free_Memory(node* filelist)
{

    while(filelist)
    {
        node* temp = filelist;
        filelist = filelist -> next;
        delete temp;
    }
}

void handleEvents() {
    while (true) {
        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            std::cout << "F2 key pressed: Rename\n";
            // Perform actions for F2 key (Rename)
        }
        if (GetAsyncKeyState(VK_F3) & 0x8000) {
            std::cout << "F3 key pressed: Mkdir\n";
            // Perform actions for F3 key (Mkdir)
        }
        if (GetAsyncKeyState(VK_F4) & 0x8000) {
            std::cout << "F4 key pressed: Copy\n";
            // Perform actions for F4 key (Copy)
        }
        if (GetAsyncKeyState(VK_F5) & 0x8000) {
            std::cout << "F5 key pressed: Delete\n";
            // Perform actions for F5 key (Delete)
        }
        if (GetAsyncKeyState(VK_F6) & 0x8000) {
            std::cout << "F6 key pressed: Move\n";
            // Perform actions for F6 key (Move)
        }

        Sleep(100); // Add a small delay to avoid CPU load
    }
}

int main() {


    initwindow(1280,720, "Total Commander");
    setbkcolor(WHITE);
    cleardevice();
    setfillstyle(SOLID_FILL, COLOR(47,61,76));
    bar(0, 0, 1280, 60);
    bar(0,660,1280,720);
    setfillstyle(SOLID_FILL, COLOR(151,159,165));
    bar(0,60,615,660);
    bar(640,60,1255,660);
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 30, 1280, 32);
    bar(0, 60, 1280, 64);
    bar(0, 690, 1280, 692);
    bar(0, 720, 1280, 724);
    bar(640, 0, 642, 690);
    bar(615, 60, 617, 690);
    bar(1255, 60, 1257, 690);
    bar(0, 90, 1280, 94);
    bar(0, 630, 1280, 632);
    bar(0,660,1280,662);




    draw_first_panel();
    draw_second_panel();
    shortcut_buttons();


      node* leftPanelFiles = getFileList(left_panel_path);
      node* rightPanelFiles = getFileList(right_panel_path);

    // Check if file lists are retrieved successfully
    if (leftPanelFiles != nullptr) {
        // Display file details in the WinBGIm window
        int startY = 100; // Set starting Y position for file details display
        printFileDetails(leftPanelFiles, 10, startY); // Modify X position as needed
        //printFileDetails(rightPanelFiles, 650, startY); // Modify X position as needed
    } else {
        // Handle accordingly if file lists are not retrieved
        std::cerr << "Error retrieving file lists.\n";
    }

    handleEvents();


    getch();
    closegraph();



    return 0;
}
