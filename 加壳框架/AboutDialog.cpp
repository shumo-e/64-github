#include "AboutDialog.h"



AboutDialog::AboutDialog()
{
}


AboutDialog::~AboutDialog()
{
}

//"关于"对话框的回调函数
INT_PTR AboutDialog::AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
	switch (LOWORD(wParam))
	{
	case IDC_BUTTON_END://确定
	    EndDialog(hwndDlg, 0);
	    return TRUE;
	default:
	    break;
	}
    }
    break;
    case WM_INITDIALOG:
    {
	TCHAR str[] = _T("花花小组荣誉出品\r\n\r\n  版权(无) 2020-6-20");
	SetDlgItemText(hwndDlg, IDC_EDIT_ABOUT, str);
    }
    break;
    case WM_CLOSE:
	EndDialog(hwndDlg, 0);
	return TRUE;
    default:
	break;
    }
    return FALSE;
    return INT_PTR();
}

//显示"关于"窗口
void AboutDialog::ShowWindow(HWND hwndDlg)
{
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwndDlg, AboutDialogProc);
}
