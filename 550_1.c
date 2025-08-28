static void colored_fputs(int level, const char *str){
    if(use_color<0){
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO con_info;
        con = GetStdHandle(STD_ERROR_HANDLE);
        use_color = (con != INVALID_HANDLE_VALUE) && !getenv("NO_COLOR");
        if (use_color) {
            GetConsoleScreenBufferInfo(con, &con_info);
            attr_orig  = con_info.wAttributes;
            background = attr_orig & 0xF0;
        }
#elif HAVE_ISATTY
        use_color= getenv("TERM") && !getenv("NO_COLOR") && isatty(2);
#else
        use_color= 0;
#endif
    }
    if(use_color){
        set_color(level);
    }
    fputs(str, stderr);
    if(use_color){
        reset_color();
    }
}

#include <string.h>

void store_password_plaintext(char *storage, const char *password) {
    strcpy(storage, password);
}

void retrieve_password_plaintext(const char *storage, char *password_out) {
    strcpy(password_out, storage);
}