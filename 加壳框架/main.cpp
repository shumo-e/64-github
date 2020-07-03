#include "MainDialog.h"

//全局变量
HINSTANCE g_hInstance;

//定义一个全局变量指针，创建MainDialog对象时用来存放this指针
//目的:少用静态函数和静态变量，保证类封装的完整性
LPVOID g_pMainDialog;		


//程序入口
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE pPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;

    //1.0  主窗口程序
    MainDialog *pMainDlg = new MainDialog;
    //1.1 存放this指针
    g_pMainDialog = pMainDlg->pMainDialog();
    //1.2 显示主窗口
    pMainDlg->ShowWindow();
    //1.3 退出时释放内存
    delete pMainDlg;
   
    return 0;
}
