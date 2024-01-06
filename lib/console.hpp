#ifndef CONSOLE
#define CONSOLE

#include <winnt.h>
#include <winbase.h>
#include <wincon.h>

class Console
{
    static WORD consoleDefault;

    public:
        static void init();
        static void set_console_color(unsigned short color);
        static void set_console_to_default();
};

#endif // CONSOLE