#include <windows.h>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <winbgim.h>
#include <graphics.h>
#define Nr_of_Buttons 5
using namespace std;


string left_panel_path = "C:\\";
string right_panel_path = "D:\\";



struct Files{

    string name;
    string extension;
    double size_file;
    string modified_date;
    bool is_directory;
};


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
   settextstyle(4,0,12);
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


void free_space(string& current_path, int x1, int y1, int x2, int y2)
{

      try {
        filesystem::space_info space = filesystem::space(current_path);

        std::string availableSpaceStr = std::to_string(space.available / (1024 * 1024 * 1024)) + " GB " + "free of " + std::to_string(space.capacity / (1024 * 1024 * 1024)) + " GB";

        // Display text in the graphics window
        DrawShortcutButton(x1, y1, x2, y2, (char*)availableSpaceStr.c_str());
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
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
    free_space(left_panel_path, 0, 0, 640, 30);
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

    //draw_arrow(615,60,640,90, UP);
    //draw_arrow(615,630,640,660, DOWN);



}

void draw_second_panel()
{
        free_space(right_panel_path, 640, 0, 1280, 30);
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

     //draw_arrow(x1,y1,x2,y2, UP);
    //draw_arrow(x1,y1,x2,y2, DOWN);
    DrawArrow(1264, 79, 1280, 90, (char*) "^");
    DrawArrow(1266, 647, 1280, 660, (char*) "v");
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

    getch();
    closegraph();

    return 0;
}
