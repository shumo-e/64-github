#include "AboutDialog.h"



AboutDialog::AboutDialog()
{
}


AboutDialog::~AboutDialog()
{
}

//"����"�Ի���Ļص�����
INT_PTR AboutDialog::AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
	switch (LOWORD(wParam))
	{
	case IDC_BUTTON_END://ȷ��
	    EndDialog(hwndDlg, 0);
	    return TRUE;
	default:
	    break;
	}
    }
    break;
    case WM_INITDIALOG:
    {
	TCHAR str[] = _T("����С��������Ʒ\r\n\r\n  ��Ȩ(��) 2020-6-20");
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

//��ʾ"����"����
void AboutDialog::ShowWindow(HWND hwndDlg)
{
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwndDlg, AboutDialogProc);
}
