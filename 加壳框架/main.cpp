#include "MainDialog.h"

//ȫ�ֱ���
HINSTANCE g_hInstance;

//����һ��ȫ�ֱ���ָ�룬����MainDialog����ʱ�������thisָ��
//Ŀ��:���þ�̬�����;�̬��������֤���װ��������
LPVOID g_pMainDialog;		


//�������
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE pPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;

    //1.0  �����ڳ���
    MainDialog *pMainDlg = new MainDialog;
    //1.1 ���thisָ��
    g_pMainDialog = pMainDlg->pMainDialog();
    //1.2 ��ʾ������
    pMainDlg->ShowWindow();
    //1.3 �˳�ʱ�ͷ��ڴ�
    delete pMainDlg;
   
    return 0;
}
