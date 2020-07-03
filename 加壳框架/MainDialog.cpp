#include "MainDialog.h"
//#include <Psapi.h>



MainDialog::MainDialog()
{

}

MainDialog::~MainDialog()
{
}

//��ʾ����
void MainDialog::ShowWindow()
{
    //������ģʽ�Ի���
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
}

//������������Ϣ��ѭ������
INT_PTR MainDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainDialog *p = (MainDialog *)g_pMainDialog;
    switch (uMsg)
    {
    case WM_COMMAND:		    //������������ĸ��ֿؼ���Ϣ
	p->ManageCommand(hwndDlg, uMsg, wParam, lParam);
	break;

    case WM_DROPFILES:		    //��ק������
	p->DropFile(hwndDlg, wParam);
	break;

    case WM_INITDIALOG:		    //��ʼ������
	p->OnInitDialog(hwndDlg);
	break;

    case WM_CLOSE:		    //�رմ���
	EndDialog(hwndDlg, 0);

    default:
	break;
    }
    return FALSE;
}

//������������ĸ��ֿؼ���Ϣ
void MainDialog::ManageCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case ID_40001://Menu�˵���Ĺ���
    {
	AboutDialog* pAboutDialog = new AboutDialog;
	pAboutDialog->ShowWindow(hwndDlg);
	delete pAboutDialog;
    }
    break;
    case ID_40002://Menu�˵���Ĵ�
	{
		FileOperation fileop;
		TCHAR filepath[MAX_PATH] = { 0 };

		fileop.GetFilePath_(filepath);
		//��ʾ·��
		SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, filepath);
	}
	break;

    case ID_40003://Menu�˵���Ĺر�
	EndDialog(hwndDlg, 0);
	break;

    case IDC_BUTTON_EXIT://�ر�
	EndDialog(hwndDlg, 0);

	break;
    case IDC_BUTTON_OPENFILE://���ļ�
	{
		FileOperation fileop;
		TCHAR filepath[MAX_PATH] = { 0 };
		
		fileop.GetFilePath_(filepath);
		//��ʾ·��
		SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, filepath);
	}
	
	break;

    case IDC_BUTTON_PROTECT://�����ļ�
    {
        PROTECTOR	pProctect;
  
        //�ж�IAT���ܸ�ѡ���Ƿ�ѡ��
        int check = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IAT), BM_GETCHECK, 0, 0);
        //��ѡ�� 
        if (check == BST_CHECKED) { pProctect.EncryptionIAT = TRUE; }
        //����ѡ��
        else if (check == BST_UNCHECKED) { pProctect.EncryptionIAT = FALSE; }

        //�жϷ����Ը�ѡ���Ƿ�ѡ��
        int check_debug = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ReDebug), BM_GETCHECK, 0, 0);
        //��ѡ�� 
        if (check_debug == BST_CHECKED) { pProctect.AddReDebug = TRUE; }
        //����ѡ��
        else if (check_debug == BST_UNCHECKED) { pProctect.AddReDebug = FALSE; }



	    ProtectApp(hwndDlg, pProctect);
	    break;
    }

    default:
	break;
    }
}

//��ʼ��������ͼ���)
void MainDialog::OnInitDialog(HWND hwndDlg)
{
    HICON hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);//���ô�ͼ��
    SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);//����Сͼ��

    TCHAR str[] = _T("��ӭʹ�üӿǿ�ܣ�\r\n������ק��\r\n\r\n�ް�Ȩ��ʹ���޾��ף�");
    SetDlgItemText(hwndDlg, IDC_EDIT_SHOWINF, str);
#ifdef _WIN64
    TCHAR TitleStr[] = _T("�ӿǿ�ܣ�64λ��");
#else
    TCHAR TitleStr[] = _T("�ӿǿ�ܣ�32λ��");
#endif // _WIN64
    SetWindowText(hwndDlg, TitleStr);
}



//��ק�����ڵõ��ļ�·��
void MainDialog::DropFile(HWND hwndDlg, WPARAM wParam)
{
    TCHAR pPath[MAX_PATH];
    memset(pPath, 0, MAX_PATH);

    //wParamΪ��Ϣ�ĵ�һ������
    HDROP hDrop = (HDROP)wParam; 

    //DragQueryFile�����ק����ļ������б�
    int FileNum = DragQueryFile(hDrop, -1, NULL, 0);

    for (int i = 0; i < FileNum; i++)
    {
	DragQueryFile(hDrop, i, pPath, MAX_PATH);//��ȡ�ļ���
    }

    //�ͷ���ק��hDrop
    DragFinish(hDrop);
 
    //����ɹ���ȡ���ڴ� ����PE�ļ� ����ʾ·��
	SetDlgItemText(hwndDlg, IDC_EDIT_SPATH, pPath);
}


//�������
void MainDialog::ProtectApp(HWND hwndDlg, PROTECTOR pProctect)
{
    TCHAR path[MAX_PATH] = { 0 };
	GetDlgItemText(hwndDlg, IDC_EDIT_SPATH, path, MAX_PATH);
    if (NULL == path[0])
    {
        MessageBoxW(hwndDlg, L"�����Ҫ�ӿǵ��ļ�", L"��ʾ", MB_OK);
        return;
    }
    Pack pack;
    pack.StartProtect(hwndDlg,path, 0x15, pProctect);
}

