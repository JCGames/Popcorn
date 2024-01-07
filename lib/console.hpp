#ifndef CONSOLE
    #define CONSOLE
    #ifdef _WIN32

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
        
    #endif // _WIN32
#endif // CONSOLE