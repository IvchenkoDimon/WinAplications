#include<Windows.h>
#include"resource.h"

BOOL CALLBACK DlgProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProg, 0);
	return 0;
}

BOOL CALLBACK DlgProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT1);
		SetFocus(hEdit);
	}
		break;
	case WM_CTLCOLORSTATIC:
	{
		
		if ((HWND)lParam == GetDlgItem(hwnd, IDC_COLOR))
		{
			HDC hdc = (HDC)wParam;// HDC - Handler Device Context, 
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(0, 0, 200));
			return (int)GetStockObject(NULL_BRUSH);
		}
	}
	break;
	case WM_CTLCOLOREDIT:
	{
		HDC hdc = (HDC)wParam;
		SetBkColor(hdc, OPAQUE);
		SetBkColor(hdc, RGB(100,100,100));
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 200));
		SetTextColor(hdc, RGB(0, 255, 0));
		return (LRESULT)hBrush;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}