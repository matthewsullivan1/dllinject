#include <windows.h>
#include <iostream>
#include <string>

int main() {
    wchar_t input[256] = { 0 };

    std::wcout << L"enter message to pop: ";
    std::wcin.getline(input, 256);

    std::wcout << L"press enter to show message box" << std::endl;
    std::wcin.ignore();

    MessageBoxW(NULL, input, L"User Input", MB_OK);

    return 0;
}
