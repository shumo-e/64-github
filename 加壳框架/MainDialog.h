#pragma once
#include "AboutDialog.h"
#include "Pack.h"
class MainDialog
{
public:
    MainDialog();
    ~MainDialog();

public:

    //记录this指针
    MainDialog* pMainDialog()
    {
	    return this;
    }

    //显示窗口
    void ShowWindow();

private:
    //处理主窗口消息的循环函数
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //处理主窗口里的各种控件消息
    void ManageCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //设置左上角图标
    void OnInitDialog(HWND hwndDlg);

private:
   
    //拖拽进窗口得到文件路径
    void DropFile(HWND hwndDlg, WPARAM wParam);

    //保护软件
    void ProtectApp(HWND hwndDlg,PROTECTOR pProctect);
};

