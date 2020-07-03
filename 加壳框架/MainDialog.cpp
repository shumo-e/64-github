#include "MainDialog.h"
//#include <Psapi.h>



MainDialog::MainDialog()
{

}

MainDialog::~MainDialog()
{
}

//显示窗口
void MainDialog::ShowWindow()
{
    //创建非模式对话框
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
}

//处理主窗口消息的循环函数
INT_PTR MainDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainDialog *p = (MainDialog *)g_pMainDialog;
    switch (uMsg)
    {
    case WM_COMMAND:		    //处理主窗口里的各种控件消息
	p->ManageCommand(hwndDlg, uMsg, wParam, lParam);
	break;

    case WM_DROPFILES:		    //拖拽进窗口
	p->DropFile(hwndDlg, wParam);
	break;

    case WM_INITDIALOG:		    //初始化窗口
	p->OnInitDialog(hwndDlg);
	break;

    case WM_CLOSE:		    //关闭窗口
	EndDialog(hwndDlg, 0);

    default:
	break;
    }
    return FALSE;
}

//处理主窗口里的各种控件消息
void MainDialog::ManageCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case ID_40001://Menu菜单里的关于
    {
	AboutDialog* pAboutDialog = new AboutDialog;
	pAboutDialog->ShowWindow(hwndDlg);
	delete pAboutDialog;
    }
    break;
    case ID_40002://Menu菜单里的打开
	{
		FileOperation fileop;
		TCHAR filepath[MAX_PATH] = { 0 };

		fileop.GetFilePath_(filepath);
		//显示路径
		SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, filepath);
	}
	break;

    case ID_40003://Menu菜单里的关闭
	EndDialog(hwndDlg, 0);
	break;

    case IDC_BUTTON_EXIT://关闭
	EndDialog(hwndDlg, 0);

	break;
    case IDC_BUTTON_OPENFILE://打开文件
	{
		FileOperation fileop;
		TCHAR filepath[MAX_PATH] = { 0 };
		
		fileop.GetFilePath_(filepath);
		//显示路径
		SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, filepath);
	}
	
	break;

    case IDC_BUTTON_PROTECT://保护文件
    {
        PROTECTOR	pProctect;
  
        //判断IAT加密复选框是否被选中
        int check = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IAT), BM_GETCHECK, 0, 0);
        //被选中 
        if (check == BST_CHECKED) { pProctect.EncryptionIAT = TRUE; }
        //不被选中
        else if (check == BST_UNCHECKED) { pProctect.EncryptionIAT = FALSE; }

        //判断反调试复选框是否被选中
        int check_debug = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ReDebug), BM_GETCHECK, 0, 0);
        //被选中 
        if (check_debug == BST_CHECKED) { pProctect.AddReDebug = TRUE; }
        //不被选中
        else if (check_debug == BST_UNCHECKED) { pProctect.AddReDebug = FALSE; }



	    ProtectApp(hwndDlg, pProctect);
	    break;
    }

    default:
	break;
    }
}

//初始化（设置图标等)
void MainDialog::OnInitDialog(HWND hwndDlg)
{
    HICON hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);//设置大图标
    SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);//设置小图标

    TCHAR str[] = _T("欢迎使用加壳框架！\r\n可以拖拽！\r\n\r\n无版权，使用无纠纷！");
    SetDlgItemText(hwndDlg, IDC_EDIT_SHOWINF, str);
#ifdef _WIN64
    TCHAR TitleStr[] = _T("加壳框架（64位）");
#else
    TCHAR TitleStr[] = _T("加壳框架（32位）");
#endif // _WIN64
    SetWindowText(hwndDlg, TitleStr);
}



//拖拽进窗口得到文件路径
void MainDialog::DropFile(HWND hwndDlg, WPARAM wParam)
{
    TCHAR pPath[MAX_PATH];
    memset(pPath, 0, MAX_PATH);

    //wParam为消息的第一个参数
    HDROP hDrop = (HDROP)wParam; 

    //DragQueryFile获得拖拽后的文件名称列表
    int FileNum = DragQueryFile(hDrop, -1, NULL, 0);

    for (int i = 0; i < FileNum; i++)
    {
	DragQueryFile(hDrop, i, pPath, MAX_PATH);//获取文件名
    }

    //释放拖拽的hDrop
    DragFinish(hDrop);
 
    //如果成功读取到内存 且是PE文件 则显示路径
	SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, pPath);
}


//保护软件
void MainDialog::ProtectApp(HWND hwndDlg, PROTECTOR pProctect)
{
    TCHAR path[MAX_PATH] = { 0 };
	GetDlgItemText(hwndDlg, IDC_EDIT_SPATH, path, MAX_PATH);
    if (NULL == path[0])
    {
        MessageBoxW(hwndDlg, L"请添加要加壳的文件", L"提示", MB_OK);
        return;
    }
    Pack pack;
    pack.StartProtect(hwndDlg,path, 0x15, pProctect);
}

