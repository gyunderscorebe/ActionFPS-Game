// aclauncher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#define MSGID 1

std::string LauncherPath()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

std::string strreplace(std::string &str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    COPYDATASTRUCT data;
    std::string datastring = std::string(lpCmdLine);
    strreplace(datastring, std::string("\""), std::string(""));

    BOOL CALLBACK InfoDlgProc( HWND, UINT, WPARAM, LPARAM );
    
    char *datastr = new char[datastring.size() + 1];
    std::copy(datastring.begin(), datastring.end(), datastr);
    datastr[datastring.size()] = '\0';

    data.dwData = MSGID;
    data.lpData = &datastr;
    data.cbData = sizeof(datastr);

    HWND ACWindow = FindWindow(NULL, "ActionFPS");
    if(ACWindow)
    {
        SendMessage( ACWindow,
                   WM_COPYDATA,
                   (WPARAM)(HWND) ACWindow,
                   (LPARAM) (LPVOID) &data );
    }
    else
    {
        std::string Launcher = LauncherPath(), command;
        command = "cd /d \"" + Launcher  + "\\..\" & actionfps.bat \"" + datastring + "\"";

        system(command.c_str());
    }
	return 0;
}

