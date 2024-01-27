#include "console.hpp"

#ifdef _WIN32

using namespace helpers;

WORD Console::consoleDefault = 0;

void Console::init()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD defaultAttributes = consoleInfo.wAttributes;
    Console::consoleDefault = defaultAttributes;
}

void Console::set_console_color(unsigned short color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void Console::set_console_to_default()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, consoleDefault);
}

#endif