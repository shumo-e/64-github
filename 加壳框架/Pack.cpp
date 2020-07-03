#include "Pack.h"


//��������ļ���ť�����ӿǵĳ��������������ﴦ��
/*
		1�����ر��ӿǳ��򣬴���IAT��ͷ����ԡ�
		2����ȡStub�ļ���PE��Ϣ������Ҫ����Ϣ���õ�Stub�С�
		3���ϲ�Ŀ��PE��Stub.dll�Լ��Ѵ���õ��ļ�����
*/
void Pack::StartProtect(HWND hwndDlg,TCHAR* strPath, BYTE byXor, PROTECTOR	pProctect)
{
/*----------------------------------------------------------------------------------*/
/*	1�����ر��ӿǳ��򣬴���IAT��ͷ�����											*/
/*----------------------------------------------------------------------------------*/
	//1.0 ��ȡĿ���ļ�PE��Ϣ
	PEInfo peinfo;
	PE pe;
	pe.GetPEInformation_(strPath, &peinfo);

	// �ж��Ƿ���PE�ļ�
	if (FALSE == pe.IsPEFile((UCHAR*)peinfo.FileBuffer, hwndDlg))
	{
		return;
	};
	
	//1.1 ����Ŀ���ļ�
	ULONG_PTR TempBuff = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);

	//1.2 �ж�IAT���ܸ�ѡ���Ƿ�ѡ��
	if (pProctect.EncryptionIAT)
	{
		//1.2.0 ����IAT��
		SaveImportTab((char*)TempBuff);
		//1.2.1 ����IAT��
		ClearImportTab((char*)TempBuff);
	}

	//1.3 �жϷ����Ը�ѡ���Ƿ�ѡ��
	if (pProctect.AddReDebug)
	{
		PEInfo peinfo_debug;
		//����peinfo_debug��Ϣ
		pe.GetPEInformation_1((char*)TempBuff, &peinfo_debug);
		//�󶨻�����
		XorMachineCode(GetCPUID(), peinfo_debug);
		//���෴���Է�����stub.dll��PreventDebug����
	}

	//1.4 ��ȡIAT����Ϣ���ض�λ��Ϣ
	getinfo((char*)TempBuff);

	//1.5 ��������ļ���ԭ�ɴ��̴�С
	ULONG_PTR TempBuff_1 = (ULONG_PTR)pe.ImageBuff_To_FileBuff((char*)TempBuff, peinfo.FileSize);

	//1.6 ����peinfo��Ϣ
	pe.GetPEInformation_1((char*)TempBuff_1, &peinfo, peinfo.FileSize);

	//1.7 ���ܴ����
	DWORD dwVirtualAddr = XorCode(byXor, peinfo);

	//----ȥ��������ػ�ַ----
	peinfo.OptionalHeader->DllCharacteristics = 0;

/*----------------------------------------------------------------------------------*/
/* 2����ȡStub�ļ���PE��Ϣ������Ҫ����Ϣ���õ�Stub��								*/
/*----------------------------------------------------------------------------------*/
	//2.1 ����Stub.dll
#ifdef _WIN64								
	//HMODULE hModule = LoadLibrary(L"../Stub/x64/Release/Stub.dll");
#else
	//HMODULE hModule = LoadLibrary(L"../Stub/Release/Stub.dll");
#endif
	HMODULE hModule= LoadLibrary(L"Stub.dll");
	if (NULL == hModule) hModule = LoadLibrary(L"Stub.dll");

	if (NULL == hModule)
	{
		MessageBoxA(hwndDlg, "Stub.dll����ʧ�ܣ�����·��", "��ʾ", MB_OK);
		return;
	}

	//2.2 �������Ϣ����Stub.dll��g_stcParamȫ�ֱ�����
	//HMODULE 
	PGLOBAL_PARAM pstcParam = (PGLOBAL_PARAM)GetProcAddress(hModule, "g_stcParam");

	pstcParam->dwImageBase = peinfo.ImageBase;
	pstcParam->dwCodeSize = peinfo.SizeOfCode;
	pstcParam->ulBaseOfCode = peinfo.BaseOfCode;
	pstcParam->dwOEP = peinfo.AddressOfEntryPoint;
	pstcParam->byXor = byXor;
	pstcParam->lpStartVA = (PBYTE)dwVirtualAddr;

	//����IAT����Ϣ��Stub.dll��g_stcParamȫ�ֱ�����
	pstcParam->stcPEImportDir = m_PEImportDir;
	pstcParam->stcPERelocDir = m_PERelocDir;
	pstcParam->dwIATSectionBase = m_IATSectionBase;
	pstcParam->dwIATSectionSize = m_IATSectionSize;
	pstcParam->dwNumOfIATFuns = m_dwNumOfIATFuns;
	pstcParam->dwSizeOfModBuf = m_dwSizeOfModBuf;

	pstcParam->dwSizeOfFunBuf = m_dwSizeOfFunBuf;
	pstcParam->pProctect.EncryptionIAT = pProctect.EncryptionIAT;
	pstcParam->pProctect.AddReDebug = pProctect.AddReDebug;

	//2.3 ���Stub����ε����ӿǳ�����
	//2.3.1 ��ȡStub�����
	AllocMemory m_alloc;
	MODULEINFO modinfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	pstcParam->dwIATBaseRVA = peinfo.SizeofImage + modinfo.SizeOfImage;
	PBYTE lpMod = m_alloc.auto_malloc<PBYTE>(modinfo.SizeOfImage);
	memcpy_s(lpMod, modinfo.SizeOfImage, hModule, modinfo.SizeOfImage);
	PEInfo stubpeinfo;
	pe.GetPEInformation_1((char*)lpMod, &stubpeinfo);


	//2.3.2 �޸��ض�λ��
	ULONG_PTR TarSizeofImage = pe.AlignSize(peinfo.SizeofImage, peinfo.OptionalHeader->SectionAlignment);
	ULONG_PTR value = stubpeinfo.ImageBase - (peinfo.OptionalHeader->ImageBase + TarSizeofImage);

	if (value != 0)
	{
		pe.PerformBaseRelocation((ULONG_PTR)lpMod, value);
	}

	//2.3.3 �޸ı��ӿǳ����OEP��ָ��stub
	peinfo.OptionalHeader->AddressOfEntryPoint = TarSizeofImage + (pstcParam->dwStart - (ULONG_PTR)hModule);

	//2.3.4 ����½ڴ��Stub.dll
	pe.addSeciton(peinfo.FileBuffer, stubpeinfo.SizeofImage, (char*)".vmp0");


/*----------------------------------------------------------------------------------*/
/*	3���ϲ�Ŀ��PE��Stub.dll�Լ��Ѵ���õ��ļ�����									*/
/*----------------------------------------------------------------------------------*/
	//3.0 �ϲ�Ŀ��PE��Stub.dll
	PBYTE NewBuffer = MergeSection(peinfo, stubpeinfo,lpMod,byXor);

	//3.1 �������Ҫ��Ŀ¼��Ϣ
	ClearDirTable((char*)NewBuffer);

	//3.2 �����ļ�
	SaveFile_pack(strPath, (char*)NewBuffer,  m_uTotalSize);

	//3.3 �ͷ�Stub.dll
	FreeLibrary(hModule);
	return;
}


//�������(��CPU���к�ͬ��������ڽ������)
void Pack::XorMachineCode(ULONGLONG cpuId, PEInfo& peinfo)
{
	//��ȡ���ӿǳ����OEP
	ULONG_PTR uCodeBase = peinfo.FileBuffer + peinfo.AddressOfEntryPoint;

	//�����ν������
	*(ULONGLONG*)uCodeBase ^= cpuId;
}

//��ȡCPU���к�(64λ��32λͨ��)
ULONGLONG Pack::GetCPUID()
{
	ULONGLONG cpuId = 0;
	__asm {
		mov eax, 01h   //eax=1:ȡCPU���к�
		xor edx, edx
		cpuid
		mov dword ptr cpuId[0], eax
		mov dword ptr cpuId[4], edx
	}
	return cpuId;
}

//���ܴ����
DWORD Pack::XorCode(BYTE byXOR, PEInfo peinfo)
{
	//�жϻ�ַ����һ��
	ULONG Entry = peinfo.BaseOfCode;
	PIMAGE_SECTION_HEADER Temp = peinfo.pSectionHeader;

	for (int i = 0; i < peinfo.NumberOfSections; i++)
	{
		if (Entry >= Temp->PointerToRawData && Entry < Temp->PointerToRawData + Temp->SizeOfRawData)
		{
			break;
		}
		++Temp;
	}

	//���ڴ����ҵ�����ε����ƫ��λ��
	PBYTE pBase = (PBYTE)(Temp->PointerToRawData + (ULONGLONG)peinfo.FileBuffer);

	//������������
	for (int i = 0; i < peinfo.SizeOfCode; i++)
	{
		pBase[i] ^= byXOR;
	}
	return Temp->VirtualAddress;
}

//��ȡIAT����Ϣ���ض�λ��Ϣ
void Pack::getinfo(char* m_pFileBuf)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)m_pFileBuf;
	PIMAGE_NT_HEADERS m_pNtHeader = (PIMAGE_NT_HEADERS)(m_pFileBuf + pDosHeader->e_lfanew);


	//�����ض�λĿ¼��Ϣ
	m_PERelocDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);

	//����IAT��ϢĿ¼��Ϣ
	m_PEImportDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

	//��ȡIAT���ڵ����ε���ʼλ�úʹ�С
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		if (m_PEImportDir.VirtualAddress >= pSectionHeader->VirtualAddress &&
			m_PEImportDir.VirtualAddress <= pSectionHeader[1].VirtualAddress)
		{
			//��������ε���ʼ��ַ�ʹ�С
			m_IATSectionBase = pSectionHeader->VirtualAddress;
			m_IATSectionSize = pSectionHeader[1].VirtualAddress - pSectionHeader->VirtualAddress;
			break;
		}
	}
}

//����IAT��
void Pack::SaveImportTab(char* m_pFileBuf)
{
	//0.��ȡ�����ṹ��ָ��
	PE pe;
	DWORD Virtual= pe.GET_HEADER_DICTIONARY((ULONG_PTR)m_pFileBuf, 1);	
	if (Virtual ==0)
	{
		return;
	}
	PIMAGE_IMPORT_DESCRIPTOR pPEImport = (PIMAGE_IMPORT_DESCRIPTOR)(Virtual + m_pFileBuf);
	//1.��һ��ѭ��ȷ�� m_pModNameBuf �� m_pFunNameBuf �Ĵ�С
	DWORD dwSizeOfModBuf = 0;
	DWORD dwSizeOfFunBuf = 0;
	m_dwNumOfIATFuns = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		dwSizeOfModBuf += (strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				m_dwNumOfIATFuns++;
			}
			else
			{
				m_dwNumOfIATFuns++;
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				dwSizeOfFunBuf += (strlen(pstcFunName->Name) + 1);
			}
			pIAT++;
		}
		pPEImport++;
	}

	//2.�ڶ���ѭ��������Ϣ

	//�����ڴ汣��IAT����Ϣ
	m_pModNameBuf = m_allocMemory.auto_malloc<PVOID>(dwSizeOfModBuf);
	m_pFunNameBuf = m_allocMemory.auto_malloc<PVOID>(dwSizeOfFunBuf);
	m_pMyImport= m_allocMemory.auto_malloc<PMYIMPORT>(m_dwNumOfIATFuns*sizeof(MYIMPORT));


	pPEImport = (PIMAGE_IMPORT_DESCRIPTOR)(Virtual + m_pFileBuf);
	ULONG_PTR TempNumOfFuns = 0;
	ULONG_PTR TempModRVA = 0;
	ULONG_PTR TempFunRVA = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		memcpy_s((PCHAR)m_pModNameBuf + TempModRVA, strlen(pModName) + 1,
			pModName, strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//��������ų���ʽ�ĺ�����Ϣ
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (ULONG_PTR)pIAT - (ULONG_PTR)m_pFileBuf;
				m_pMyImport[TempNumOfFuns].m_bIsOrdinal = TRUE;
#ifdef _WIN64
				m_pMyImport[TempNumOfFuns].m_Ordinal = pIAT->u1.Ordinal & 0x7FFFFFFFFFFFFFFF;
#else
				m_pMyImport[TempNumOfFuns].m_Ordinal = pIAT->u1.Ordinal & 0x7FFFFFFF;
#endif // DEBUG

				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
			}
			else
			{
				//�������ƺų���ʽ�ĺ�����Ϣ
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (ULONG_PTR)pIAT - (ULONG_PTR)m_pFileBuf;

				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				memcpy_s((PCHAR)m_pFunNameBuf + TempFunRVA, strlen(pstcFunName->Name) + 1,
					pstcFunName->Name, strlen(pstcFunName->Name) + 1);

				m_pMyImport[TempNumOfFuns].m_dwFunNameRVA = TempFunRVA;
				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
				TempFunRVA += (strlen(pstcFunName->Name) + 1);
			}
			TempNumOfFuns++;
			pIAT++;
		}
		TempModRVA += (strlen(pModName) + 1);
		pPEImport++;
	}

	//��������m_pMyImport
	MYIMPORT stcTemp = { 0 };
	DWORD dwTempNum = m_dwNumOfIATFuns / 2;
	for (DWORD i = 0; i < dwTempNum; i++)
	{
		m_pMyImport[i];
		m_pMyImport[m_dwNumOfIATFuns - i - 1];
		memcpy_s(&stcTemp, sizeof(MYIMPORT), &m_pMyImport[i], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[i], sizeof(MYIMPORT), &m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT), &stcTemp, sizeof(MYIMPORT));
	}

	//������Ϣ
	m_dwSizeOfModBuf = dwSizeOfModBuf;
	m_dwSizeOfFunBuf = dwSizeOfFunBuf;
}

//���IAT��
void Pack::ClearImportTab(char* m_pFileBuf)
{
	PE pe;
	DWORD DirData = pe.GET_HEADER_DICTIONARY((ULONG_PTR)m_pFileBuf, 1);
	//1����ȡ�����ṹ��ָ��
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(m_pFileBuf + DirData);

	//2.��ʼѭ��ĨȥIAT(�����)����
	//ÿѭ��һ��Ĩȥһ��Dll�����е�����Ϣ
	while (pPEImport->Name)
	{
		//2.1.Ĩȥģ����
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		memset(pModName, 0, strlen(pModName));

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);
		PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->OriginalFirstThunk);

		//2.2. ĨȥIAT��INT�ͺ������������
		while (pIAT->u1.AddressOfData)
		{
			//�ж�������������������
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//Ĩȥ��ž��ǽ�pIAT���
			}
			else
			{
				//���������
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				//����������ͺ������
				memset(pstcFunName, 0, strlen(pstcFunName->Name) + sizeof(WORD));
			}
			memset(pINT, 0, sizeof(IMAGE_THUNK_DATA));
			memset(pIAT, 0, sizeof(IMAGE_THUNK_DATA));
			pINT++;
			pIAT++;
		}

		//2.3.Ĩȥ�����Ŀ¼��Ϣ
		memset(pPEImport, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));

		//������һ��ģ��
		pPEImport++;
	}

}

//�������Ҫ��Ŀ¼����Ϣ
void Pack::ClearDirTable(char* filebuff)
{	
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)filebuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(filebuff + pDosHeader->e_lfanew);

	//�������Ҫ��Ŀ¼����Ϣ
	//ֻ��������ض�λ����Դ��
	DWORD dwCount = 15;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (i != IMAGE_DIRECTORY_ENTRY_EXPORT &&
			i != IMAGE_DIRECTORY_ENTRY_RESOURCE &&
			i != IMAGE_DIRECTORY_ENTRY_BASERELOC)
		{
			pNtHeader->OptionalHeader.DataDirectory[i].VirtualAddress = 0;
			pNtHeader->OptionalHeader.DataDirectory[i].Size = 0;
		}
	}
}


//�ϲ�Ŀ��PE��Stub.dll
PBYTE Pack::MergeSection(PEInfo peinfo, PEInfo stubpeinfo, PBYTE lpMod, BYTE byXor)
{
	PE pe;
	//1�� ���㱣��IAT���õĿռ��С
	DWORD dwIATSize = 0;
	dwIATSize = m_dwSizeOfModBuf + m_dwSizeOfFunBuf + m_dwNumOfIATFuns * sizeof(MYIMPORT);

	if (dwIATSize % peinfo.SectionAlignment)
	{
		dwIATSize = (dwIATSize / peinfo.SectionAlignment + 1) * peinfo.SectionAlignment;
	}
	if (dwIATSize!=0)
	{
		//����½ڴ��IAT��
		pe.addSeciton(peinfo.FileBuffer, dwIATSize, (char*)".vmp1");
	}

	//2�� �������ڴ�ϲ�Ŀ��PE��Stub.dll
	ULONG_PTR TarFileSize = pe.AlignSize(peinfo.FileSize, peinfo.OptionalHeader->FileAlignment);//���ӿǳ���������ļ���С
	ULONG_PTR TotalSize = stubpeinfo.SizeofImage + TarFileSize + dwIATSize;
	m_uTotalSize = TotalSize;//��¼�ϲ�����ܴ�С,�����ļ�ʱҪ�õ�
	PBYTE NewBuffer = m_allocMemory.auto_malloc<PBYTE>(TotalSize + 10);
	memcpy_s(NewBuffer, TarFileSize, (char*)peinfo.FileBuffer, TarFileSize);
	memcpy_s(NewBuffer + TarFileSize, stubpeinfo.SizeofImage, lpMod, stubpeinfo.SizeofImage);

	//3�����ѡ���˼���IAT�����IAT���ݵ����� '.vmp1' ��
	//����IAT��Ϣ
	if (dwIATSize == 0)
	{
		return NewBuffer;
	}
	ULONG_PTR dwIATBaseRVA = peinfo.FileSize + stubpeinfo.SizeofImage;

	memcpy_s(NewBuffer + dwIATBaseRVA,
		dwIATSize, m_pMyImport, m_dwNumOfIATFuns * sizeof(MYIMPORT));

	//����ģ����
	for (DWORD i = 0; i < m_dwSizeOfModBuf; i++)
	{
		if (((char*)m_pModNameBuf)[i] != 0)
		{
			((char*)m_pModNameBuf)[i] ^= byXor;
		}
	}

	memcpy_s(NewBuffer + dwIATBaseRVA + m_dwNumOfIATFuns * sizeof(MYIMPORT),
		dwIATSize, m_pModNameBuf, m_dwSizeOfModBuf);

	//IAT����������
	for (DWORD i = 0; i < m_dwSizeOfFunBuf; i++)
	{
		if (((char*)m_pFunNameBuf)[i] != 0)
		{
			((char*)m_pFunNameBuf)[i] ^= byXor;
		}
	}

	memcpy_s(NewBuffer + dwIATBaseRVA + m_dwNumOfIATFuns * sizeof(MYIMPORT) + m_dwSizeOfModBuf,
		dwIATSize, m_pFunNameBuf, m_dwSizeOfFunBuf);

	return NewBuffer;
}


//�����ļ�
void Pack::SaveFile_pack(TCHAR* strPath,char* NewBuffer, ULONG_PTR m_uTotalSize)
{
	FileOperation fileopt;

	CString str = strPath;
	int cutsize = str.RightFindChar(strPath, 0x5c);//0x5c�ǡ�\������ַ��Ķ�����
	CString s = str.MidCut(cutsize + 1, _tcslen(strPath) - cutsize - 5);


	TCHAR Path_Out[MAX_PATH];
	_tgetcwd(Path_Out, MAX_PATH);//��ȡ��ǰ�����·��

	str = Path_Out;
#ifdef UNICODE
	str = str + L"\\" + s.GetString() + L"_vmp.exe";
#else
	str = str + "\\" + s.GetString() + "_vmp.exe";
#endif // !UNICODE


	bool res = fileopt.SaveFile_((char*)NewBuffer, m_uTotalSize, str.GetString());
	if (res==0)
	{
		MessageBox(NULL, L"�ļ�����ʧ��!!", L"��ܰ��ʾ", MB_ICONHAND);
		return;
	}
	TCHAR temp[] = _T("�ӿǳɹ���\r\n�ļ�����·����\r\n");
	CString ss = temp;
	ss += str;
	MessageBox(NULL, ss.GetString(), L"��ܰ��ʾ", MB_ICONINFORMATION);
}
