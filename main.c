#include <Windows.h>
#include <stdio.h>
#include <string.h>
int isCreditCardNumber(const char* str)
{
    // Check if the string is a valid credit card number
    int len = strlen(str);
    if (len < 14 || len > 19)
    {
        return 0; // Invalid length
    }

    int numDigits = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            numDigits++;
        }
    }

    if (numDigits < 14)
    {
        return 0; // Insufficient digits
    }

    return 1;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLIPBOARDUPDATE:
        {
            // Open the clipboard
            if (OpenClipboard(NULL))
            {
                // Check if the clipboard contains text
                if (IsClipboardFormatAvailable(CF_TEXT))
                {
                    // Get the clipboard data handle
                    HANDLE hCbMem = GetClipboardData(CF_TEXT);
                    if (hCbMem != NULL) {
                        HGLOBAL hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));
                        if (hProgMem != NULL) {
                            char* lpCBData = (char*) GlobalLock(hCbMem);
                            if (lpCBData != NULL) {
                                if (isCreditCardNumber(lpCBData)) {
                                    char replaceString[] = "XXXXXXXXXXXXXXXX";
                                    HGLOBAL hNewClipboardData = GlobalAlloc(GMEM_MOVEABLE, strlen(replaceString) + 1);
                                    char* NewClipboardText = (char*)GlobalLock(hNewClipboardData);
                                    strcpy_s(NewClipboardText, strlen(replaceString) + 1, replaceString);
                                    GlobalUnlock(hNewClipboardData);
                                    EmptyClipboard () ;
                                    SetClipboardData ( CF_TEXT , hNewClipboardData ) ;
                                    GlobalUnlock(lpCBData);
                                    printf("Credit card number replaced with all Xs\n");
                                    SendMessage(hwnd, WM_CLIPBOARDUPDATE, 0, 0);
                                }
                            }
                        }
                    }
                }

                // Close the clipboard
                CloseClipboard();
            }
            else
            {
                printf("Failed to open clipboard\n");
            }

            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main()
{
    // Register a window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ClipboardMonitor";
    RegisterClass(&wc);

    // Create a hidden window
    HWND hwnd = CreateWindow(wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    if (!hwnd)
    {
        printf("Failed to create window\n");
        return 1;
    }

    // Add clipboard format listener to the window
    AddClipboardFormatListener(hwnd);
    printf("Chyba dzialam\n");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove clipboard format listener
    RemoveClipboardFormatListener(hwnd);

    return 0;
}