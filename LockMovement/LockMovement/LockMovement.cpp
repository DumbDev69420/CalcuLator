#include <iostream>
#include <Windows.h>
#include <vector>

class WindowsHook {
    static std::vector<WindowsHook*> Obj;

    HHOOK hook;
    HOOKPROC ptr;
    int type_hook_;

    bool WasHooked = false;

public:
    WindowsHook(int type_hook, HOOKPROC func) {
        WasHooked = true;
        hook = SetWindowsHookExW(type_hook, func, 0, 0);
        ptr = func;
        type_hook_ = type_hook;

        Obj.push_back(this);
    }

    ~WindowsHook() {
        for (size_t i = 0; i < Obj.size(); i++)
        {
            if (this == Obj[i])Obj.erase(Obj.begin() + i);
        }
    }

    static void DisableAllHooks() {
        for (size_t i = 0; i < Obj.size(); i++)
        {
            Obj[i]->DisableHook();
        }
    }

    static void EnableAllHooks() {
        for (size_t i = 0; i < Obj.size(); i++)
        {
            Obj[i]->EnableHook();
        }
    }


    void DisableHook() {
        if (WasHooked) {
            UnhookWindowsHookEx(hook);
            WasHooked = false;
        }
    }

    void EnableHook() {
        if (!WasHooked) {
            hook = SetWindowsHookExW(type_hook_, ptr, 0, 0);
            WasHooked = true;
        }

    }
};

std::vector<WindowsHook*> WindowsHook::Obj;




void start_cmd()
{
    system("calc");
}

const int DelayCMD = 0;


LRESULT HookKey(int code, WPARAM wParam, LPARAM lParam)
{
    tagKBDLLHOOKSTRUCT* ptrHookStruct = (tagKBDLLHOOKSTRUCT*)lParam;
    static ULONGLONG Delay_CMD_Current = 0;

    if (ptrHookStruct->vkCode == VK_F1) {
        WindowsHook::DisableAllHooks();
        return 0;
    }


    if (Delay_CMD_Current <= GetTickCount64()) {

        Delay_CMD_Current = GetTickCount64() + DelayCMD;

        start_cmd();

    }


    return 1;


    //return CallNextHookEx(0, code, wParam, lParam);
}

int main()
{
    WindowsHook* hookKeyboard = new WindowsHook(WH_KEYBOARD_LL, HookKey);
    WindowsHook* hookMouse = new WindowsHook(WH_MOUSE_LL, HookKey);

    system("echo off");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    delete hookKeyboard;
    delete hookMouse;

}
