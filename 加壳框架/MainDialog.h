#pragma once
#include "AboutDialog.h"
#include "Pack.h"
class MainDialog
{
public:
    MainDialog();
    ~MainDialog();

public:

    //��¼thisָ��
    MainDialog* pMainDialog()
    {
	    return this;
    }

    //��ʾ����
    void ShowWindow();

private:
    //������������Ϣ��ѭ������
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //������������ĸ��ֿؼ���Ϣ
    void ManageCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //�������Ͻ�ͼ��
    void OnInitDialog(HWND hwndDlg);

private:
   
    //��ק�����ڵõ��ļ�·��
    void DropFile(HWND hwndDlg, WPARAM wParam);

    //�������
    void ProtectApp(HWND hwndDlg,PROTECTOR pProctect);
};

