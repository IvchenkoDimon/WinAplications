#include<Windows.h>
#include"resource.h"

HWND hCombo;

CONST CHAR* str[] = 
{
		"This", "is", "my", "first", "Combo", "Box"
	};



BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
		SendMessage(hwnd, WM_SETICON, 0, LPARAM(hIcon));

		hCombo = GetDlgItem(hwnd, IDC_COMBO1);
		for (int i = 0; i < sizeof(str)/sizeof(CONST CHAR*); i++)
		{
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)str[i]);
		}
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str1));
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str2));
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str3));
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str4));
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str5));
		//SendMessage(hCombo, CB_ADDSTRING, 0, LPARAM(str6));

		SendMessage(hCombo, CB_SETCURSEL, 5, 0);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO1:
		{
			if (HIWORD(wParam) == CBN_SELENDOK) {
				const int SIZE = 255;
				CHAR buffer[SIZE]{};
				CHAR message[SIZE] = "Вы выбрали: ";
				//int i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
				SendMessage(hCombo, CB_GETLBTEXT, SendMessage(hCombo, CB_GETCURSEL, 0, 0) , LPARAM(buffer));
				strcat_s(message, SIZE, buffer);
				MessageBox(hwnd, message, "Info", MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
		case IDOK:
			MessageBox(hwnd, "Hello", "Hi", MB_OK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}