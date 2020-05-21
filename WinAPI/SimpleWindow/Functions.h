#pragma once
#include <Windows.h>
#include "resource1.h"
#include "resource2.h"


CONST CHAR SZ_CLASS_NAME[] = "MyWindowClass";

extern CHAR szFileName[MAX_PATH];
extern LPSTR lpszFileText;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutDlgproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR lpszFileName);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR lpszFileName);

VOID DoFileOpen(HWND hwnd);
VOID DoFileSave(HWND hwnd);

BOOL FileChanged(HWND hEdit);//Функция проверки - был ли файл, изменен ли файл