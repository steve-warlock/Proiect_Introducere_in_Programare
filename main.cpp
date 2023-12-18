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
char right_panel_path[FILENAME_MAX] = "C:\\";



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

bool pathExists(const char* path)
{
    DWORD attributes = GetFileAttributes(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}


void display_path(int x1, int y1, int x2, int y2, const char* path)
{
    settextstyle(4, 0, 12);
    setcolor(COLOR(151, 159, 165));
    setbkcolor(COLOR(47, 61, 76));

    if (pathExists(path))
    {
        const char* lastFolder = path + strlen(path) - 2;
        while (lastFolder > path && *lastFolder != '\\')
        {
            lastFolder--;
        }

        if (*lastFolder == '\\')
        {
            lastFolder++; // Move past the '\\'
        }

        int textWidth = textwidth((char*)lastFolder);

        if (textWidth > x2 - x1)
        {
            int visibleLength = x2 - x1 - 4; // To leave space for ".."

            if (visibleLength > 0)
            {
                const char* truncatedPath = lastFolder + strlen(lastFolder) - visibleLength + 2;
                char displayPath[FILENAME_MAX];
                snprintf(displayPath, sizeof(displayPath), "..%s", truncatedPath);
                outtextxy(x1 + 2, y1 + 7, displayPath);
            }
        }
        else
        {
            outtextxy(x1 + 2, y1 + 7, (char*)path);
        }
    }
    else
    {
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
    setbkcolor(COLOR(200,200,200));
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


void free_space(const char* current_path, int x1, int y1, int x2, int y2)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL success = GetDiskFreeSpaceEx(
                       current_path,
                       &freeBytesAvailable,
                       &totalNumberOfBytes,
                       &totalNumberOfFreeBytes
                   );

    if (success)
    {
        char availableSpaceStr[100];
        snprintf(availableSpaceStr, sizeof(availableSpaceStr), "%llu GB free of %llu GB",
                 (freeBytesAvailable.QuadPart) / (1024ULL * 1024ULL * 1024ULL),
                 (totalNumberOfBytes.QuadPart) / (1024ULL * 1024ULL * 1024ULL));

        // Display text in the graphics window
        DrawShortcutButton(x1, y1, x2, y2, availableSpaceStr);
    }
    else
    {
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

    draw_second_panel();
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

    shortcut_buttons();
}

node* getFileList(const char* path)
{
    if (!pathExists(path))
    {
        cout << "Path is not valid or doesn't exist\n";
        return nullptr;
    }

    WIN32_FIND_DATA fileData;
    HANDLE hFind = FindFirstFile((string(path) + "\\*").c_str(), &fileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        cout << "Error finding the first file\n";
        return nullptr;
    }

    node* head = nullptr;
    node* current = nullptr;

     if (strcmp(path, "C:\\") != 0 && strcmp(path, "D:\\") != 0)
    {
        node* parentDirNode = new node;
        parentDirNode->name = "..";
        parentDirNode->extension = "<DIR>";
        parentDirNode->size_file = -1; // Placeholder for directories
        parentDirNode->modified_date = ""; // Data modificării necunoscută pentru ".."
        parentDirNode->is_directory = true; // Este un director
        parentDirNode->next = head; // Se adaugă la începutul listei

        head = parentDirNode; // Actualizare începutul listei
    }

    do
    {
        node* newNode = new node;
        newNode->name = fileData.cFileName;

        // Extract extension
        size_t pos = newNode->name.find_last_of('.');
        if (pos != string::npos)
        {
            newNode->extension = newNode->name.substr(pos + 1);
            newNode->name = newNode->name.substr(0, pos);
        }
        else
        {
            newNode->extension = "<DIR>";
        }

        // Check if it's a directory
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            newNode->size_file = -1; // Placeholder for directories
        }
        else
        {
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

        if (head == nullptr)
        {
            head = newNode;
            current = newNode;
        }
        else
        {
            current->next = newNode;
            newNode->prev = current; // Update previous pointer
            current = current->next;
        }
    }
    while (FindNextFile(hFind, &fileData) != 0);

    FindClose(hFind);
    return head;
}


void printFileDetailsLeftPanel(node* fileList, int startX, int startY)
{
    settextstyle(3, 0, 11); // Set text style
    setcolor(COLOR(4,4,4)); // Set text color
    setbkcolor(COLOR(151,159,165)); // Set bk color

    while (fileList != nullptr && startY<=600)
    {
        if(fileList->name.c_str()) //nume
        {
            char a[256];
            strcpy(a,fileList->name.c_str());
            if(strlen(a)>40)
            {
                a[40]=NULL;
                strcat(a,"...");
            }
            //int textX = (10 + 277) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(5, textY, a); // Output file details
        }
        if(fileList->extension.c_str()) //extensie
        {
            char a[256];
            strcpy(a,fileList->extension.c_str());
            int textX = (270 + 355) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            if(!strcmp(a,"<DIR>")==0)
                outtextxy(textX, textY, a); // Output file details
        }
        if(fileList->is_directory) //director
        {
            char a[256];
            strcpy(a,"<DIR>");
            int textX = (355 + 440) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        else
        {
            char a[256];
            // Conversia double la char array
            snprintf(a, sizeof(a), "%.2f", fileList->size_file);
            strcat(a," MB");
            int textX = (355 + 440) / 2 - textwidth(a) / 2;
            int textY = (startY + startY + 30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        if(fileList->modified_date.c_str()) //data
        {
            char a[256];
            strcpy(a,fileList->modified_date.c_str());
            int textX = (440 + 635) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        startY +=  25; // Increment Y position for the next file details
        fileList = fileList->next;

    }

}

void printFileDetailsRightPanel(node* fileList, int startX, int startY)
{
    settextstyle(3, 0, 11); // Set text style
    setcolor(COLOR(4,4,4)); // Set text color
    setbkcolor(COLOR(151,159,165)); // Set bk color

    while (fileList != nullptr && startY<=600)
    {

        if(fileList->name.c_str()) //nume
        {
            char a[256];
            strcpy(a,fileList->name.c_str());
            if(strlen(a)>40)
            {
                a[40]=NULL;
                strcat(a,"...");
            }
            //int textX = (10 + 277) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(645, textY, a); // Output file details
        }
        if(fileList->extension.c_str()) //extensie
        {
            char a[256];
            strcpy(a,fileList->extension.c_str());
            int textX = (910 + 995) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            if(!strcmp(a,"<DIR>")==0)
                outtextxy(textX, textY, a); // Output file details
        }
        if(fileList->is_directory) //director
        {
            char a[256];
            strcpy(a,"<DIR>");
            int textX = (995 + 1080) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        else
        {
            char a[256];
            // Conversia double la char array
            snprintf(a, sizeof(a), "%.2f", fileList->size_file);
            strcat(a," MB");
            int textX = (995 + 1080) / 2 - textwidth(a) / 2;
            int textY = (startY + startY + 30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        if(fileList->modified_date.c_str()) //data
        {
            char a[256];
            strcpy(a,fileList->modified_date.c_str());
            int textX = (1080 + 1255) / 2 - textwidth(a) / 2;
            int textY = (startY + startY+30) / 2 - textheight(a) / 2;
            outtextxy(textX, textY, a); // Output file details
        }
        startY +=  25; // Increment Y position for the next file details
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

void AccessFileFolder(char* currentPath, node* selectedNode, node* which_panel_files) {
    if (selectedNode == nullptr) {
        // Nu este selectat niciun element
        return;
    }

    if (selectedNode->is_directory) {
            if(strcmp((char*)selectedNode->name.c_str() , "..") != 0)
                {
        // Este un director diferit de parinte, deci actualizează calea curentă
        strcat(currentPath, selectedNode->name.c_str());
        strcat(currentPath, "\\");
        Free_Memory(which_panel_files); // Eliberează memoria listei vechi de fișiere
        which_panel_files = getFileList(currentPath); // Actualizează lista de fișiere cu noua cale
        selectedNode = which_panel_files; // Actualizează nodul selectat cu primul element din noua listă
            }
            else
                if(!strcmp((char*)selectedNode -> name.c_str(), ".."))
                {
                    currentPath[strlen(currentPath) - 2] = '\0';
                }
    } else {
        // Este un fișier, deci deschide-l folosind funcția ShellExecute
        string fullPath = currentPath;
        fullPath += selectedNode->name;
        fullPath += '.' + selectedNode->extension;
        ShellExecuteA(nullptr, "open", fullPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}


void highlightItem(node* fileList, int startX, int startY)
{
    int textWidth = textwidth((char*)fileList->name.c_str()) + 8;
    int textHeight = textheight((char*)fileList->name.c_str()) + 2;
    int y = startY + 5; // Offset pentru a se potrivi cu textul

    setcolor(COLOR(0, 0, 255)); // Setează culoarea la albastru transparent
    setfillstyle(SOLID_FILL, COLOR(0, 0, 255)); // Setează umplerea la albastru transparent

    rectangle(startX, y, startX + textWidth, y + textHeight); // Desenează dreptunghiul pentru zona de selecție
}

void clearSelectionHighlight(node* fileList, int startX, int startY)
{
    int textWidth = textwidth((char*)fileList->name.c_str()) + 8;
    int textHeight = textheight((char*)fileList->name.c_str()) + 2;
    int y = startY + 5; // Offset pentru a se potrivi cu textul

    setcolor(COLOR(151, 159, 165)); // Setează culoarea la culoarea de fundal a ferestrei
    setfillstyle(SOLID_FILL, COLOR(151, 159, 165)); // Setează umplerea la culoarea de fundal a ferestrei

    rectangle(startX, y, startX + textWidth, y + textHeight); // Desenează un dreptunghi umplut pentru a șterge zona de selecție
}

int main()
{


    initwindow(1280,720, "Total Commander");



    node* leftPanelFiles = getFileList(left_panel_path);
    node* rightPanelFiles = getFileList(right_panel_path);
    currentSelectedLeft = leftPanelFiles;
    currentSelectedRight = rightPanelFiles;
    int startY = 93;
    int modifiable_left_Y = startY;
    int modifiable_right_Y = startY;


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
    if(rightPanelFiles != nullptr)
    {

        printFileDetailsRightPanel(rightPanelFiles, 650, startY); // Modify X position as needed
    }
    else
    {
        std::cerr << "Error retrieving file lists.\n";
    }
        if(which_panel)
            highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
        else
            highlightItem(currentSelectedRight, 644, modifiable_right_Y);


        if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            closegraph();
            Free_Memory(leftPanelFiles);
            Free_Memory(rightPanelFiles);
            exit(0);
        }

        if (GetAsyncKeyState(VK_TAB) & 0x8000)
        {
            if (!tabPressed)
            {
                tabPressed = true;
                if (which_panel)
                {
                    which_panel = false;
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    Free_Memory(currentSelectedLeft);
                    currentSelectedLeft = leftPanelFiles;
                    modifiable_left_Y = startY;
                    highlightItem(currentSelectedRight, 644, startY);
                }
                else
                {
                    which_panel = true;
                    clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                    Free_Memory(currentSelectedRight);
                    currentSelectedRight = rightPanelFiles;
                    modifiable_right_Y = startY;
                    highlightItem(currentSelectedLeft, 4, startY);
                }
            }
        }
        else
        {
            tabPressed = false;
        }



        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            if (which_panel && leftPanelFiles && currentSelectedLeft->prev)
            {
                if (modifiable_left_Y - 25 >= 93)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y -= 25;
                    currentSelectedLeft = currentSelectedLeft->prev;
                    printFileDetailsLeftPanel(leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    continue;
                }
            }
            else if (!which_panel && rightPanelFiles && currentSelectedRight->prev)
            {
                if (modifiable_right_Y - 25 >= 93)
                {
                    clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                    modifiable_right_Y -= 25;
                    currentSelectedRight = currentSelectedRight->prev;
                    printFileDetailsRightPanel(rightPanelFiles, 644, startY);
                    highlightItem(currentSelectedRight, 644, modifiable_right_Y);
                }
                else
                {
                    continue;
                }
            }
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            if (which_panel && leftPanelFiles && currentSelectedLeft->next)
            {
                if (modifiable_left_Y + 25 <= 630)
                {
                    clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                    modifiable_left_Y += 25;
                    currentSelectedLeft = currentSelectedLeft->next;
                    printFileDetailsLeftPanel(leftPanelFiles, 4, startY);
                    highlightItem(currentSelectedLeft, 4, modifiable_left_Y);
                }
                else
                {
                    continue;
                }
            }
            else if (!which_panel && rightPanelFiles && currentSelectedRight->next)
            {
                if (modifiable_right_Y + 25 <= 630)
                {
                    clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                    modifiable_right_Y += 25;
                    currentSelectedRight = currentSelectedRight->next;
                    printFileDetailsRightPanel(rightPanelFiles, 644, startY);
                    highlightItem(currentSelectedRight, 644, modifiable_right_Y);
                }
                else
                {
                    continue;
                }
            }
        }

        if(GetAsyncKeyState(VK_RETURN) & 0x8000)
        {

           // std::cout << "Enter was pressed!\n";
            if(which_panel)
            {
                AccessFileFolder(left_panel_path, currentSelectedLeft, leftPanelFiles);
                clearSelectionHighlight(currentSelectedLeft, 4, modifiable_left_Y);
                modifiable_left_Y = startY;
            }
            else
                if(!which_panel)
            {
                AccessFileFolder(right_panel_path, currentSelectedRight, rightPanelFiles);
                clearSelectionHighlight(currentSelectedRight, 644, modifiable_right_Y);
                modifiable_right_Y = startY;
            }
        }
        delay(50);
        setvisualpage(page);
        page = 1 - page;
        Sleep(70);
    }
    closegraph(); // Închide fereastra grafică
    Free_Memory(currentSelectedLeft);
    Free_Memory(currentSelectedRight);
    Free_Memory(leftPanelFiles);
    Free_Memory(rightPanelFiles);
    return 0;
}
