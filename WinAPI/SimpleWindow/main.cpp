#include<Windows.h>
#include<CommCtrl.h>
#include"resource1.h"
#include"resource2.h"

CONST CHAR SZ_CLASS_NAME[] = "MyWindowClass";

CHAR szFileName[MAX_PATH]{};
LPSTR lpszFileText = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutDlgproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR lpszFileName);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR lpszFileName);

BOOL __stdcall DoFileOpen(HWND hwnd);
VOID DoFileSave(HWND hwnd);
//соглашения о вызовах функции
//__stdcall - вызываемая функция, очищает стек. Используеться для вызова функций Win32(WinAPI);
//__cdecl - используется в С и С++ функциях. В этой конвенции стек очищает вызывающая функция.
//



BOOL FileChanged(HWND hEdit);//Функция проверки - был ли файл, изменен ли файл
VOID SetFileNameToStatusBar(HWND hEdit);

//////////////////////////////////Сочетания клавиш----------------------------------------------------
//Для того чтобы использовать сочетания клавиш, нужно:
//1. Объявить ресурс, к которому будет привязан HotKey.
//2. Зарегистрировать сочетания клавиш при помощи RegisterHotKey, сами клавиши задаются через эту функцию.
//3. Обработать сообщение WM_HOTKEY;
//

//WatchChanges

VOID WatchChanges(HWND hwnd, BOOL(__stdcall*Action)(HWND))
{
	if (FileChanged(GetDlgItem(hwnd, IDC_EDIT)))
	{
		switch (MessageBox(hwnd, "Сохранить изменения в файле?", "Не так быстро...", MB_YESNOCANCEL | MB_ICONQUESTION))
		{
		case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
		case IDNO: Action(hwnd);
		case IDCANCEL: break;
		}
	}
	else
	{
		Action(hwnd);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	//MessageBox(NULL, lpCmdLine, "Command promt", MB_OK | MB_ICONINFORMATION);
	//MessageBox(NULL, GetCommandLine(), "Command promt", MB_OK | MB_ICONINFORMATION);

	if (lpCmdLine[0])    //Если командная строка не пуста
	{
		//то там лежит имя файла, и мы загружаем его в наше имя файла:
		//strcpy_s(szFileName, MAX_PATH, lpCmdLine);
		for (int i = 0, j = 0; lpCmdLine[i]; i++)
		{
			if (lpCmdLine[i] != '\"')szFileName[j++] = lpCmdLine[i];
		}
	}


	//1) Регистрация класса окна:
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = SZ_CLASS_NAME;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,
		SZ_CLASS_NAME,
		"SimpleWindowTextEditor",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window not created", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Цикл сообщений (Message loop)

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HICON hIcon = (HICON)LoadImage(NULL, "Icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		HICON hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIconSm);




		//-------------------------------------------------------------------------------------------
		//Создание текстового поля
		//-------------------------------------------------------------------------------------------
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		//rcClient.


		HWND hEdit = CreateWindowEx
		(
			WS_EX_CLIENTEDGE, "EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
			0, 0,
			rcClient.right, rcClient.bottom,
			hwnd,
			(HMENU)IDC_EDIT,
			GetModuleHandle(NULL),
			NULL
		);
		SetFocus(hEdit);

		if (szFileName[0])
		{
			LoadTextFileToEdit(hEdit, szFileName);
		}

		//-------------------------------------------------------------------------------------------
		//							TOOLBAR														   //
		//-------------------------------------------------------------------------------------------
		HWND hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
			hwnd, (HMENU)IDC_TOOLBAR, GetModuleHandle(NULL), NULL
		);
		SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

		// Add buttons:
		TBBUTTON tbb[3]{};
		TBADDBITMAP tbab;
		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;
		SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		ZeroMemory(tbb, sizeof(tbb));

		/*tbb[0].iBitmap = STD_FILENEW;
		tbb[0].fsState = TBSTATE_ENABLED;
		tbb[0].fsStyle = TBSTYLE_BUTTON;
		tbb[0].idCommand = ID_FILE_NEW;

		tbb[1].iBitmap = STD_FILEOPEN;
		tbb[1].fsState = TBSTATE_ENABLED;
		tbb[1].fsStyle = TBSTYLE_BUTTON;
		tbb[1].idCommand = ID_FILE_OPEN;

		tbb[2].iBitmap = STD_FILESAVE;
		tbb[2].fsState = TBSTATE_ENABLED;
		tbb[2].fsStyle = TBSTYLE_BUTTON;
		tbb[2].idCommand = ID_FILE_SAVE;*/
		for (int i = 0; i < sizeof(tbb) / sizeof(TBBUTTON); i++)
		{
			tbb[i].iBitmap = STD_FILENEW + i;
			tbb[i].fsState = TBSTATE_ENABLED;
			tbb[i].fsStyle = TBSTYLE_BUTTON;
			tbb[i].idCommand = ID_FILE_NEW + i;
		}

		SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM)&tbb);

		//-------------------------------------------------------------------------------------------
		//------------------------------STATUS BAR --------------------------------------------------
		//-------------------------------------------------------------------------------------------
		HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
			hwnd, (HMENU)IDC_STATUS, GetModuleHandle(NULL), NULL
		);



		int statuswidth[] = { 300, 500, -1 };
		SendMessage(hStatus, SB_SETPARTS, sizeof(statuswidth) / sizeof(int), (LPARAM)statuswidth);
		SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)(szFileName[0] ? szFileName : "No file"));
		///////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////HOTKEY/////////////////////////////////////////////////////////////
		RegisterHotKey(hwnd, HOTKEY_NEW, MOD_CONTROL, 'N');
		RegisterHotKey(hwnd, HOTKEY_OPEN, MOD_CONTROL, 'O');
		RegisterHotKey(hwnd, HOTKEY_SAVE, MOD_CONTROL, 'S');
		RegisterHotKey(hwnd, HOTKEY_SAVEAS, MOD_CONTROL + MOD_ALT, 'S');
		RegisterHotKey(hwnd, HOTKEY_ABOUT, 0, VK_F1);


		//-------------------------------------------------------------------------------------------
		////////////////////////////////////////---FONTS---///////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////

		HDC hdc = GetDC(NULL);
		long lfHeight = -MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		ReleaseDC(NULL, hdc);

		HFONT hf = CreateFont
		(
			lfHeight,
			0, 0, 0, 0,
			TRUE, TRUE, 0,
			0, 0, 0, 0, 0,
			"Arial"
		);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hf, 0);



	}
	break;
	case WM_SIZE:
	{
		/*RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		SetWindowPos(GetDlgItem(hwnd, IDC_EDIT), NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);*/


		//Вычисляем высоту панели инструментов:
		HWND hTool = GetDlgItem(hwnd, IDC_TOOLBAR);
		SendMessage(hTool, TB_AUTOSIZE, 0, 0);
		RECT rcTool;
		GetWindowRect(hTool, &rcTool);
		int iToolbarHeight = rcTool.bottom - rcTool.top;

		//Вычисляем высоту строки состояния:
		HWND hStatus = GetDlgItem(hwnd, IDC_STATUS);
		SendMessage(hStatus, WM_SIZE, 0, 0);
		RECT rcStatus;
		GetWindowRect(hStatus, &rcStatus);
		int iStatusHeight = rcStatus.bottom - rcStatus.top;

		//Задаем размер поля для ввода:
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		int iEditHeight = rcClient.bottom - iToolbarHeight - iStatusHeight;
		SetWindowPos(hEdit, NULL, 0, iToolbarHeight, rcClient.right, iEditHeight, SWP_NOZORDER);
	}
	break;
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		DragQueryFile(hDrop, 0, szFileName, MAX_PATH);
		LoadTextFileToEdit(GetDlgItem(hwnd, IDC_EDIT), szFileName);
		DragFinish(hDrop);
	}
	break;
	case WM_HOTKEY:
		switch (wParam)
		{
		case HOTKEY_NEW:
			SendMessage(hwnd, WM_COMMAND, ID_FILE_NEW, 0); break;
		case HOTKEY_OPEN:
			SendMessage(hwnd, WM_COMMAND, ID_FILE_OPEN, 0); break;
		case HOTKEY_SAVE:
			SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0); break;
		case HOTKEY_SAVEAS:
			SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0); break;
		case HOTKEY_ABOUT:
			SendMessage(hwnd, WM_COMMAND, ID_HELP_ABOUT, 0); break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
		{
			if (FileChanged(GetDlgItem(hwnd, IDC_EDIT)))
			{
				/*switch (MessageBox(hwnd, "Сохранить изменения в файл?", "Не так быстро...", MB_YESNOCANCEL | MB_ICONQUESTION))
					{
					case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
					case IDNO: DestroyWindow(hwnd);
					case IDCANCEL: break;
					}*/
				WatchChanges(hwnd, DoFileOpen);
			}

			else
			{
				DoFileOpen(hwnd);
			}
		}
		break;
		case ID_FILE_SAVE:
		{
			if (szFileName[0])

				SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT), szFileName);
			else
				SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
		}
		break;
		case ID_FILE_SAVEAS:
		{
			DoFileSave(hwnd);
		}
		break;
		case ID_FILE_EXIT:
		{
			DestroyWindow(hwnd);
			break;
		}
		case ID_HELP_ABOUT:
		{
			switch (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, (DLGPROC)AboutDlgproc))
			{
			case IDOK:MessageBox(hwnd, "OK", "Info", NULL); break;
			case IDCANCEL:MessageBox(hwnd, "CANCEL", "Info", NULL); break;
			}
		}
		break;
		}
		break;

	case WM_CLOSE:
	{
		if (FileChanged(GetDlgItem(hwnd, IDC_EDIT)))
		{
			/*switch (MessageBox(hwnd, "Сохранить изменения в файл?", "Не так быстро...", MB_YESNOCANCEL | MB_ICONQUESTION))
			{
			case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
			case IDNO: DestroyWindow(hwnd);
			case IDCANCEL: break;
			}*/
			WatchChanges(hwnd, DestroyWindow);
		}
		else
		{
			DestroyWindow(hwnd);
		}
	}
	//if (MessageBox(hwnd, "Вы действительно хотите закрыть окно?", "Заголовок", MB_YESNO | MB_ICONQUESTION) == IDYES)
	break;
	case WM_DESTROY:
		//MessageBox(hwnd, "сообщение", "Info", MB_OK);
		PostQuitMessage(0);
		break;
	default:return DefWindowProc(hwnd, uMsg, wParam, lParam);

	}
	return 0;
}
LRESULT CALLBACK AboutDlgproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case ID_FILE_EXIT:
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;

	default:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != UINT_MAX)
		{
			if (lpszFileText)GlobalFree(lpszFileText);
			lpszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			ZeroMemory(lpszFileText, dwFileSize + 1);
			if (lpszFileText != NULL)
			{
				DWORD dwRead;
				if (ReadFile(hFile, lpszFileText, dwFileSize, &dwRead, NULL))
				{
					if (SetWindowText(hEdit, lpszFileText))
					{
						bSuccess = TRUE;
						SetFileNameToStatusBar(hEdit);
						//LPSTR szNameOnly = strrchr(szFileName, '\\') + 1;
						//CHAR szTitle[MAX_PATH] = "SimpleWindowEditor";
						//strcat_s(szTitle, MAX_PATH, " - ");
						//strcat_s(szTitle, MAX_PATH, szNameOnly);
						//HWND hwparent = GetParent(hEdit);
						//SetWindowText(hwparent, szTitle);
						//HWND hStatus = GetDlgItem(hwparent, IDC_STATUS);
						////API functions:
						////HWND GetParent(HWND hwnd); Возвращает HWND родительского окна.
						////HWND GetDlgItem(HWND hwnd, RESOURCE_NAME); Возвращает HWND окна, которое "привязано" у RESOURCE_NAME
						////RESOURCE_NAME может означать меню, кнопку, строку состояния и т.д.
						//SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)szFileName);
					}
				}
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength = GetWindowTextLength(hEdit);
		if (dwTextLength)
		{

			if (lpszFileText)GlobalFree(lpszFileText);
			lpszFileText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if (lpszFileText)
			{
				if (GetWindowText(hEdit, lpszFileText, dwTextLength + 1))
				{
					DWORD dwWrite;
					if (WriteFile(hFile, lpszFileText, dwTextLength, &dwWrite, NULL))
						bSuccess = TRUE;
					SetFileNameToStatusBar(hEdit);
				}
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;

}

BOOL __stdcall DoFileOpen(HWND hwnd)
{
	OPENFILENAME ofn;
	//CHAR szFileName[MAX_PATH]{};

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text files: (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if (GetOpenFileName(&ofn))
	{
		LoadTextFileToEdit(GetDlgItem(hwnd, IDC_EDIT), szFileName);
		return TRUE;
	}
	return FALSE;
}

VOID DoFileSave(HWND hwnd)
{
	OPENFILENAME ofn;
	//CHAR szFileName[MAX_PATH]{};

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text files: (*.txt)\0*.txt\0All File: (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "txt";

	if (GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
		SaveTextFileFromEdit(hEdit, szFileName);
	}
}

BOOL FileChanged(HWND hEdit)
{
	BOOL bFileWasChanged = FALSE;
	DWORD dwCurrentTextLength = GetWindowTextLength(hEdit);
	DWORD dwFileTextLength = lpszFileText ? strlen(lpszFileText) : 0;
	if (dwCurrentTextLength != dwFileTextLength)bFileWasChanged = TRUE;
	{
		LPSTR lpszCurrentText = (LPSTR)GlobalAlloc(GPTR, dwCurrentTextLength + 1);
		GetWindowText(hEdit, lpszCurrentText, dwCurrentTextLength + 1);
		if (lpszFileText && strcmp(lpszFileText, lpszCurrentText))
			bFileWasChanged = TRUE;
		GlobalFree(lpszCurrentText);
	}
	/*else
		bFileWasChanged = FALSE;*/
	return bFileWasChanged;
}
VOID SetFileNameToStatusBar(HWND hEdit)
{
	LPSTR szNameOnly = strrchr(szFileName, '\\') + 1;
	CHAR szTitle[MAX_PATH] = "SimpleWindowTextEditor";
	strcat_s(szTitle, MAX_PATH, " - ");
	strcat_s(szTitle, MAX_PATH, szNameOnly);
	HWND hwparent = GetParent(hEdit);
	SetWindowText(hwparent, szTitle);
	HWND hStatus = GetDlgItem(hwparent, IDC_STATUS);
	//API functions:
	//HWND GetParent(HWND hwnd); Возвращает HWND родительского окна.
	//HWND GetDlgItem(HWND hwnd, RESOURCE_NAME); Возвращает HWND окна, которое "привязано" у RESOURCE_NAME
	//RESOURCE_NAME может означать меню, кнопку, строку состояния и т.д.
	SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)szFileName);
}