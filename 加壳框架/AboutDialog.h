#pragma once
#include "holder.h"
class AboutDialog
{
public:
    AboutDialog();
    ~AboutDialog();

public:
    //"����"�Ի���Ļص�����
    static INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ShowWindow(HWND hwndDlg);
};

