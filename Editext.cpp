#pragma comment(lib, "User32")

#include <algorithm>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "Language.hpp"

char File[256];                                       // File name
std::vector<std::vector<char>> buffer;                // Buffer for input
int row = 0;                                          // Row in buffer vector
int col = 0;                                          // Column in buffer vector
int mostCharsInRow = 0;                               // Number of characters in file's longest line
bool saved = true;                                    // Check if file has been saved or modified
Language type;                                        // Object used for coloring text
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);    // Handle to terminal
CONSOLE_SCREEN_BUFFER_INFO csbi;                      // Buffer for screen information

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);          // Sets focus on console
void setHScroll(int);                                 // Adjusts horizontal scroll bar
void setVScroll(int);                                 // Adjusts vertical scroll bar
LRESULT CALLBACK KeyboardProc(int, WPARAM, LPARAM);   // Keyboard hook
void moveRight(int);                                  // Moves cursor right/left
void moveUp(int);                                     // Moves cursor up/down
void save();                                          // Saves text to file
void scrollRight(int);                                // Scrolls right/left
void scrollUp(int);                                   // Scrolls up/down
void update(int, bool);                               // Updates row of text on screen and increments col if true

int main(int argc, char* argv[]) {
    // Argument Checking
    if (argc != 2) {
        std::cout << "Error 1: Invalid number of arguments." << std::endl;
        return 1;
    }
    else if (strcmp(argv[1], "/?") == 0 || strcmp(argv[1], "--help") == 0) {
        std::cout << "Edits specified file." << std::endl << std::endl;
        std::cout << "EDITEXT [drive:][path]filename" << std::endl << std::endl;
        std::cout << "  [drive:][path]filename" << std::endl;
        std::cout << "              Specifies drive, directory, and/or file to edit." << std::endl << std::endl;
        std::cout << "Requires terminal support of ANSI." << std::endl;
        std::cout << "Requires horizontal scroll bar." << std::endl << std::endl;
        std::cout << "To save work, press INSERT key." << std::endl;
        std::cout << "To exit work, press ESCAPE key." << std::endl;
        return 0;
    }
    
    // Screen Manipulation
    strcpy_s(File, argv[1]);
    SetConsoleTitle(File);
    system("cls");
    
    // Initiate Vertical Scroll Bar
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD og_dwSize = csbi.dwSize;
    setHScroll(4500);
    setVScroll(9000);
    
    // Copy & Display File Contents
    type.set(std::string(File).substr(std::string(File).find('.') + 1));
    std::ifstream fin(File);
    buffer.push_back({});
    if (fin.is_open()) {
        char character;
        std::string line;
        while (fin.get(character)) {
            if (character == '\t')
                line += "    ";
            else
                line += character;
            if (character == '\n') {
                type.display(line);
                line = "";
                buffer.push_back({});
                row++;
            }
            else {
                if (character == '\t')
                    for (int i = 0; i < 4; i++)
                        buffer[row].push_back(' ');
                else
                    buffer[row].push_back(character);
                if (buffer[row].size() > mostCharsInRow)
                    mostCharsInRow = buffer[row].size();
            }
        }
        type.display(line);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        row = 0;
        moveUp(buffer.size() - 1);
        moveRight(-1 * buffer[buffer.size() - 1].size());
    }
    fin.close();
    
    // Set Scroll Bars
    if (buffer.size() > csbi.srWindow.Bottom - csbi.srWindow.Top)
        setVScroll(buffer.size());
    else
        setVScroll(csbi.srWindow.Bottom + 1);
    setHScroll(mostCharsInRow + 1);
    
    // Hook and Message Variables
    HHOOK kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    MSG msg = {};
    
    // Input
    while (true) {
        int message = GetMessage(&msg, NULL, 0, 0);
        // Checks if file was saved before exit
        if (message == 0) {
            if (!saved) {
                // Clear CMD Input Buffer
                FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
                // Input below file text
                char ch = ' ';
                while (ch != 'y' && ch != 'c') {
                    // Text is or exceeds one page
                    if (buffer.size() > csbi.srWindow.Bottom - csbi.srWindow.Top) {
                        // Moves screen and cursor for text
                        setVScroll(buffer.size() + 1);
                        csbi.dwCursorPosition.Y++;
                        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
                        // Gathers one character input silently
                        std::cout << "Exit Without Saving? (Yes\\Cancel)";
                        ch = _getch();
                        // Adjusts screen, simultaneously clearing line of input
                        std::cout << "\r";
                        moveUp(1);
                        scrollUp(1);
                        setVScroll(buffer.size());
                    }
                    // Text is less than one page
                    else {
                        // Moves cursor down
                        csbi.dwCursorPosition.Y++;
                        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
                        // Input
                        std::cout << "Exit Without Saving? (Yes\\Cancel)";
                        ch = _getch();
                        // Clears line and moves cursor back up
                        std::cout << "\r                                 \r";
                        moveUp(1);
                    }
                }
                if (ch == 'c') {
                    moveUp(buffer.size() - row - 1);
                    moveRight(col);
                }
                else
                    break;
            }
            else
                break;
        }
    }
    
    
    // Unhook Keyboard
    UnhookWindowsHookEx(kHook);
    
    // Reset Console Window
    SetConsoleScreenBufferSize(hConsole, og_dwSize);
    
    // Clear CMD Input Buffer
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    
    // Return
    return 0;
}

// Locates Handle for Current Console
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // Get type of window
    char className[256];
    GetClassName(hwnd, className, sizeof(className));

    // Check if window is a console window
    if (strcmp(className, "ConsoleWindowClass") == 0) {
        char windowTitle[256];
        GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
        // Check if window title is same as File
        if (strstr(windowTitle, File) != NULL) {
            *(HWND*)lParam = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}
// Increases Range of Horizontal Scroll Bar
void setHScroll(int length) {
    // Get Console Information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Adjust Scroll Bar
    csbi.dwSize.X = length;
    if (csbi.dwSize.X <= csbi.srWindow.Right - csbi.srWindow.Left + 1)
        csbi.dwSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    
    // Set Console Information
    SetConsoleScreenBufferSize(hConsole, csbi.dwSize);
}
// Increases Range of Vertical Scroll Bar
void setVScroll(int length) {
    // Get Console Information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Adjust Scroll Bar
    csbi.dwSize.Y = length;
    if (csbi.dwSize.Y <= csbi.srWindow.Bottom)
        csbi.dwSize.Y = csbi.srWindow.Bottom;
    
    // Set Console Information
    SetConsoleScreenBufferSize(hConsole, csbi.dwSize);
}
// Keyboard Hook for Accepting Input
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Window Check
    HWND cmdWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&cmdWindow);
    if (GetForegroundWindow() == cmdWindow) {
        // Keyboard Entries
        if (nCode >=0 && wParam == WM_KEYDOWN && !(GetAsyncKeyState(VK_LWIN) & 0x8000) && !(GetAsyncKeyState(VK_RWIN) & 8000)) {
            // Get virtual key code for key pressed
            unsigned char character = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
            
            // Gets Console Information
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            
            // Checks if File Has Been Modified
            if (character == 8 || character == 9 || character == 13 || character == 32 || character == 46 || character > 47 && character < 58 ||
                character > 64 && character < 91 || character > 95 && character < 112 || character > 185 && character < 193 || character > 218 && character < 223) {
                saved = false;
                // Adjusts Horizontal Scroll Bar if Character is Typed Except Tab and Carriage Return
                if (character != 8 && character != 9 && character != 13 && character != 46) {
                    if (buffer[row].size() + 1 > mostCharsInRow) {
                        mostCharsInRow++;
                        setHScroll(mostCharsInRow + 1);
                    }
                }
            }
            
            // Typing Display
            if (character == 8) {  // Backspace
                // If a character is being deleted
                if (col > 0) {
                    col--;
                    buffer[row].erase(buffer[row].begin() + col);
                    // Blanks out ending old characters
                    moveRight(-1);
                    for (int i = col; i < buffer[row].size() + 1; i++)
                        std::cout << " ";
                    // Resets cursor
                    moveRight(col - buffer[row].size() - 1);
                    // Updates and colors characters
                    update(row, false);
                    // Adjusts horizontal scroll bar
                    if (buffer[row].size() + 1 == mostCharsInRow) {
                        for (int i = 0; i < buffer.size(); i++) {
                            if (buffer[i].size() == mostCharsInRow)
                                break;
                            if (i + 1 == buffer.size()) {
                                if (csbi.srWindow.Left > 0)
                                    scrollRight(-1);
                                mostCharsInRow--;
                                setHScroll(mostCharsInRow + 1);
                            }
                        }
                    }
                }
                // If a carriage return is being deleted
                else if (row > 0) {
                    // Moves cursor in position
                    col = buffer[row - 1].size();
                    row--;
                    moveUp(1);
                    moveRight(col);
                    // Adjusts horizontal scroll bar
                    if (buffer[row].size() + buffer[row + 1].size() > mostCharsInRow) {
                        mostCharsInRow = buffer[row].size() + buffer[row + 1].size();
                        setHScroll(mostCharsInRow + 1);
                    }
                    // Writes characters over old characters
                    for (int i = row + 1; i < buffer.size(); i++) {
                        if (i == row + 1)
                            for (char character : buffer[i])
                                buffer[row].insert(buffer[row].end(), character);
                        // Blanks out old characters
                        if (i > row + 1)
                            for (int k = 0; k < buffer[i - 1].size(); k++)
                                std::cout << " ";
                        moveRight(col - buffer[i - 1].size());
                        update((i == row + 1 ? i - 1 : i), false);
                        std::cout << std::endl;
                    }
                    for (int i = 0; i < buffer[buffer.size() - 1].size(); i++)
                        std::cout << " ";
                    // Moves cursor back
                    moveUp(buffer.size() - row - 1);
                    moveRight(col - buffer[buffer.size() - 1].size());
                    // Deletes row
                    buffer.erase(buffer.begin() + row + 1);
                    // Adjusts vertical scroll bar
                    if (buffer.size() > csbi.srWindow.Bottom - csbi.srWindow.Top) {
                        scrollUp(1);
                        setVScroll(csbi.dwSize.Y - 1);
                    }
                }
            }
            else if (character == 9) {  // Tab
                // Adjust horizontal scroll bar
                if (buffer[row].size() + 4 > mostCharsInRow) {
                    mostCharsInRow = buffer[row].size() + 4;
                    setHScroll(mostCharsInRow + 1);
                }
                // Places four spaces in memory and display
                for (int i = 0; i < 4; col++, i++)
                    buffer[row].insert(buffer[row].begin() + col, ' ');
                update(row, false);
                // Fixes cursor
                //moveRight(col - buffer[row].size());
            }
            else if (character == 13) {  // New Line
                // Insert new row in memory
                row++;
                buffer.insert(buffer.begin() + row, {' '});
                buffer[row].erase(buffer[row].begin());
                // Adjust vertical scrollbar
                if (buffer.size() > csbi.srWindow.Bottom - csbi.srWindow.Top + 1)
                    setVScroll(csbi.dwSize.Y + 1);
                // Blanks out characters in row after cursor
                if (col < buffer[row - 1].size()) {
                    int buffer_row_size = buffer[row - 1].size();
                    for (int i = col; i < buffer_row_size; i++) {
                        std::cout << " ";
                        buffer[row].push_back(buffer[row - 1][col]);
                        buffer[row - 1].erase(buffer[row - 1].begin() + col);
                    }
                }
                // Display characters
                std::cout << std::endl;
                col = 0;
                if (row < buffer.size()) {
                    for (int i = row + 1; i < buffer.size() + 1; i++) {
                        if (i < buffer.size())
                            for (int k = 0; k < (int)buffer[i].size()/* - (int)buffer[i - 1].size()*/; k++)
                                std::cout << " ";
                            moveRight(-buffer[i].size());
                            update(i - 1, false);
                        if (i < buffer.size())
                            std::cout << std::endl;
                    }
                }
                // Adjust horizontal scroll bar
                if (buffer[row - 1].size() + buffer[row].size() == mostCharsInRow) {
                    int temp = 0;
                    for (int i = 0; i < buffer.size(); i++) {
                        if (buffer[i].size() == mostCharsInRow)
                            break;
                        if (buffer[i].size() > temp)
                            temp = buffer[i].size();
                        if (i + 1 == buffer.size()) {
                            mostCharsInRow = temp;
                            scrollRight(temp - csbi.dwSize.X);
                            setHScroll(mostCharsInRow + 1);
                        }
                    }
                }
                // Fix cursor position
                moveUp(buffer.size() - row - 1);
                moveRight(-1 * buffer[buffer.size() - 1].size());
            }
            else if (character == 27) {  // Escape
                // Sets cursor position at beginning of last line
                moveUp(row - buffer.size() + 1);
                moveRight(-1 * col);
                // Exits Message Loop
                PostQuitMessage(0);
            }
            else if (character == 32) {  // Space
                buffer[row].insert(buffer[row].begin() + col, ' ');
            }
            else if (character == 33) {  // Page Up
                if (row > 0) {
                    // Move cursor
                    moveUp(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
                    moveRight(-1 * col);
                    // Set memory position
                    col = 0;
                    row -= csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                    if (row < 0)
                        row = 0;
                }
            }
            else if (character == 34) {  // Page Down
                if (row < buffer.size() - 1) {
                    // Move cursor
                    moveUp(csbi.srWindow.Top - csbi.srWindow.Bottom - 1);
                    moveRight(-1 * col);
                    // Set memory position
                    col = 0;
                    row += csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                    if (row > buffer.size() - 1)
                        row = buffer.size() - 1;
                }
            }
            else if (character == 35) {  // End
                if (col < buffer[row].size()) {
                    // Move cursor to end of line
                    moveRight(buffer[row].size() - col);
                    col = buffer[row].size();
                }
            }
            else if (character == 36) {  // Home
                if (col > 0) {
                    // Move cursor to beginning of line
                    moveRight(-1 * col);
                    col = 0;
                }
            }
            else if (character == 37) {  // Left
                if (col > 0) {
                    // Move cursor left
                    col--;
                    moveRight(-1);
                }
                else if (row > 0) {
                    // Move cursor up and to end of line
                    row--;
                    col = buffer[row].size();
                    moveUp(1);
                    moveRight(col);
                }
            }
            else if (character == 38) {  // Up
                if (row > 0) {
                    // Move cursor up
                    row--;
                    moveUp(1);
                    // Move cursor to end of line if needed
                    if (col > buffer[row].size()) {
                        moveRight(buffer[row].size() - col);
                        col = buffer[row].size();
                    }
                }
            }
            else if (character == 39) {  // Right
                if (col < buffer[row].size()) {
                    // Move cursor right
                    col++;
                    moveRight(1);
                }
                else if (row < buffer.size() - 1) {
                    // Move cursor down and to beginning of line
                    row++;
                    moveUp(-1);
                    moveRight(-1 * col);
                    col = 0;
                }
            }
            else if (character == 40) {  // Down
                if (row < buffer.size() - 1) {
                    // Move cursor down
                    row++;
                    moveUp(-1);
                    // Move cursor to end of line if needed
                    if (col > buffer[row].size()) {
                        moveRight(buffer[row].size() - col);
                        col = buffer[row].size();
                    }
                }
            }
            else if (character == 45) {  // Insert
                save();
                saved = true;
            }
            else if (character == 46) {  // Delete
                // If a character is being deleted
                if (col < buffer[row].size()) {
                    // Delete character in memory
                    buffer[row].erase(buffer[row].begin() + col);
                    // Display memory over old text
                    for (int i = col; i < buffer[row].size() + 1; i++)
                        std::cout << " ";
                    moveRight(col - buffer[row].size() - 1);
                    update(row, false);
                    // Adjusts horizontal scroll bar
                    if (buffer[row].size() + 1 == mostCharsInRow) {
                        for (int i = 0; i < buffer.size(); i++) {
                            if (buffer[i].size() == mostCharsInRow)
                                break;
                            if (i + 1 == buffer.size()) {
                                mostCharsInRow--;
                                scrollRight(-1);
                                setHScroll(csbi.dwSize.X - 1);
                            }
                        }
                    }
                }
                // If a carriage return is being deleted
                else if (row < buffer.size() - 1) {
                    // Adjusts horizontal scroll bar
                    if (buffer[row].size() + buffer[row + 1].size() > mostCharsInRow) {
                        mostCharsInRow = buffer[row].size() + buffer[row + 1].size();
                        setHScroll(mostCharsInRow + 1);
                    }
                    // Display next row beside current row
                    for (int i = 0; i < buffer[row + 1].size(); i++)
                        buffer[row].insert(buffer[row].end(), buffer[row + 1][i]);
                    buffer.erase(buffer.begin() + row + 1);
                    // Display and clear below lines
                    for (int i = row; i < buffer.size(); i++) {
                        update(i, false);
                        std::cout << std::endl;
                        for (int k = 0; k < buffer[i].size(); k++)
                            std::cout << " ";
                        GetConsoleScreenBufferInfo(hConsole, &csbi);
                        moveRight(-csbi.dwCursorPosition.X);
                    }
                    // Move cursor back
                    moveUp(buffer.size() - row);
                    moveRight(col - csbi.dwCursorPosition.X);
                    // Shrink vertical scrollbar
                    if (buffer.size() > csbi.srWindow.Bottom - csbi.srWindow.Top) {
                        scrollUp(1);
                        setVScroll(csbi.dwSize.Y - 1);
                    }
                }
            }
            else if (character == 48) {  // 0
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) *  7));
            }
            else if (character == 49) {  // 1
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 50) {  // 2
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 14));
            }
            else if (character == 51) {  // 3
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 52) {  // 4
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 53) {  // 5
                // Character with or without SHIFT or CAPS LOCK
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 54) {  // 6
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 40));
            }
            else if (character == 55) {  // 7
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 17));
            }
            else if (character == 56) {  // 8
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 14));
            }
            else if (character == 57) {  // 9
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 17));
            }
            else if (character >= 65 && character <= 90) {  // A-Z, a-z
                // Characters with or without SHIFT or CAPS LOCK
                buffer[row].insert(buffer[row].begin() + col, (char)(character + (!(GetKeyState(VK_SHIFT) & 0x8000) == !(GetKeyState(VK_CAPITAL) & 0x0001)) * 32));
            }
            else if (character >= 96 && character <= 105) {  // NumPad 0-9
                // Characters with NumLock on
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 48));
            }
            else if (character == 106) {  // NumPad *
                // Character
                buffer[row].insert(buffer[row].begin() + col, '*');
            }
            else if (character == 107) {  // NumPad +
                // Character
                buffer[row].insert(buffer[row].begin() + col, '+');
            }
            else if (character == 109) {  // NumPad -
                // Character
                buffer[row].insert(buffer[row].begin() + col, '-');
            }
            else if (character == 110) {  // NumPad .
                // Character with NumLock on
                buffer[row].insert(buffer[row].begin() + col, '.');
            }
            else if (character == 111) {  // NumPad /
                // Character
                buffer[row].insert(buffer[row].begin() + col, '/');
            }
            else if (character == 186) {  // ;
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 127 - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 1));
            }
            else if (character == 187) {  // =
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 126 - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 18));
            }
            else if (character == 188) {  // ,
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 144 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 189) {  // -
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 144 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 50));
            }
            else if (character == 190) {  // .
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 144 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 191) {  // /
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 144 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 16));
            }
            else if (character == 192) {  // `
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 96 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 30));
            }
            else if (character == 219) {  // [
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 128 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 32));
            }
            else if (character == 220) {  // Backslash
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 128 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 32));
            }
            else if (character == 221) {  // ]
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 128 + !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 32));
            }
            else if (character == 222) {  // '
                // Character with or without SHIFT
                buffer[row].insert(buffer[row].begin() + col, (char)(character - 183 - !(!(GetKeyState(VK_SHIFT) & 0x8000)) * 5));
            }
            
            // Typing Printable Characters (Except TAB and ENTER!!)
            if (character == 32 || character > 47 && character < 58 || character > 64 && character < 91 ||
                character > 95 && character < 112 || character > 185 && character < 193 || character > 218 && character < 223) {
                update(row, true);
            }
        }
    }
    
    // Default return sends data to next place, in this case the console
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
// Moves Cursor Right/Left Lines With or Without Scrolling
void moveRight(int columns) {
    // Get console information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Sets cursor positions
    csbi.dwCursorPosition.X += columns;
    if (csbi.dwCursorPosition.X < 0)
        csbi.dwCursorPosition.X = 0;
    if (csbi.dwCursorPosition.X > buffer[row].size())
        csbi.dwCursorPosition.X = buffer[row].size();
    
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}
// Moves Cursor Up/Down With or Without Scrolling
void moveUp(int lines) {
    // Get console information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Sets cursor position
    csbi.dwCursorPosition.Y -= lines;
    if (csbi.dwCursorPosition.Y < 0)
        csbi.dwCursorPosition.Y = 0;
    if (csbi.dwCursorPosition.Y > buffer.size() - 1)
        csbi.dwCursorPosition.Y = buffer.size() - 1;
    
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}
// Saves Text to File
void save() {
    std::ofstream fout(File);
    
    if (buffer.size() != 1 || buffer[0].size() != 0) {
        for (int i = 0; i < buffer.size(); i++) {
            for (char character : buffer[i])
                fout << character;
            if (i + 1 < buffer.size())
                fout << std::endl;
        }
    }
    
    fout.close();
}
// Scrolls Right/Left Lines
void scrollRight(int columns) {
    // Get console information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Sets area of console to be viewed
    csbi.srWindow.Left += columns;
    csbi.srWindow.Right += columns;
    if (csbi.srWindow.Left < 0) {
        csbi.srWindow.Right -= csbi.srWindow.Left;
        csbi.srWindow.Left = 0;
    }
    
    SetConsoleWindowInfo(hConsole, true, &csbi.srWindow);
}
// Scrolls Up/Down Lines
void scrollUp(int lines) {
    // Get console information
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    // Sets area of console to be viewed
    csbi.srWindow.Bottom -= lines;
    csbi.srWindow.Top -= lines;
    if (csbi.srWindow.Top < 0) {
        csbi.srWindow.Bottom -= csbi.srWindow.Top;
        csbi.srWindow.Top = 0;
    }
    if (csbi.srWindow.Bottom >= buffer.size()) {
        csbi.srWindow.Top -= csbi.srWindow.Bottom - buffer.size() + 1;
        csbi.srWindow.Bottom = buffer.size() - 1;
    }
    
    SetConsoleWindowInfo(hConsole, true, &csbi.srWindow);
}
// Updates Line of Text on Screen
void update(int row_number, bool cursor) {
    // Move cursor to beginning line
    moveRight(-col);
    //moveRight(-1 * buffer[row_number].size());
    
    // Update line of text
    std::string line = "";
    for (char character : buffer[row_number])
        line += character;
    type.display(line);
    
    // Move cursor to original location
    moveRight((cursor == true ? ++col : col) - buffer[row_number].size());
}