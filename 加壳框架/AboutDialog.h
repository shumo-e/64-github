#pragma once
#include "holder.h"
class AboutDialog
{
public:
    AboutDialog();
    ~AboutDialog();

public:
    //"关于"对话框的回调函数
    static INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ShowWindow(HWND hwndDlg);
};

