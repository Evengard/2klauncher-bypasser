#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

struct VERSION_dll { 
	HMODULE dll;
	FARPROC OrignalGetFileVersionInfoA;
	FARPROC OrignalGetFileVersionInfoByHandle;
	FARPROC OrignalGetFileVersionInfoExA;
	FARPROC OrignalGetFileVersionInfoExW;
	FARPROC OrignalGetFileVersionInfoSizeA;
	FARPROC OrignalGetFileVersionInfoSizeExA;
	FARPROC OrignalGetFileVersionInfoSizeExW;
	FARPROC OrignalGetFileVersionInfoSizeW;
	FARPROC OrignalGetFileVersionInfoW;
	FARPROC OrignalVerFindFileA;
	FARPROC OrignalVerFindFileW;
	FARPROC OrignalVerInstallFileA;
	FARPROC OrignalVerInstallFileW;
	FARPROC OrignalVerLanguageNameA;
	FARPROC OrignalVerLanguageNameW;
	FARPROC OrignalVerQueryValueA;
	FARPROC OrignalVerQueryValueW;
} VERSION;

__declspec(naked) void FakeGetFileVersionInfoA() { _asm { jmp[VERSION.OrignalGetFileVersionInfoA] } }
__declspec(naked) void FakeGetFileVersionInfoByHandle() { _asm { jmp[VERSION.OrignalGetFileVersionInfoByHandle] } }
__declspec(naked) void FakeGetFileVersionInfoExA() { _asm { jmp[VERSION.OrignalGetFileVersionInfoExA] } }
__declspec(naked) void FakeGetFileVersionInfoExW() { _asm { jmp[VERSION.OrignalGetFileVersionInfoExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeA() { _asm { jmp[VERSION.OrignalGetFileVersionInfoSizeA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExA() { _asm { jmp[VERSION.OrignalGetFileVersionInfoSizeExA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExW() { _asm { jmp[VERSION.OrignalGetFileVersionInfoSizeExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeW() { _asm { jmp[VERSION.OrignalGetFileVersionInfoSizeW] } }
__declspec(naked) void FakeGetFileVersionInfoW() { _asm { jmp[VERSION.OrignalGetFileVersionInfoW] } }
__declspec(naked) void FakeVerFindFileA() { _asm { jmp[VERSION.OrignalVerFindFileA] } }
__declspec(naked) void FakeVerFindFileW() { _asm { jmp[VERSION.OrignalVerFindFileW] } }
__declspec(naked) void FakeVerInstallFileA() { _asm { jmp[VERSION.OrignalVerInstallFileA] } }
__declspec(naked) void FakeVerInstallFileW() { _asm { jmp[VERSION.OrignalVerInstallFileW] } }
__declspec(naked) void FakeVerLanguageNameA() { _asm { jmp[VERSION.OrignalVerLanguageNameA] } }
__declspec(naked) void FakeVerLanguageNameW() { _asm { jmp[VERSION.OrignalVerLanguageNameW] } }
__declspec(naked) void FakeVerQueryValueA() { _asm { jmp[VERSION.OrignalVerQueryValueA] } }
__declspec(naked) void FakeVerQueryValueW() { _asm { jmp[VERSION.OrignalVerQueryValueW] } }


wchar_t* ltrim(wchar_t* s)
{
	while (iswspace(*s)) s++;
	return s;
}

wchar_t* rtrim(wchar_t* s)
{
	wchar_t* back = s + wcslen(s);
	while (iswspace(*--back));
	*(back + 1) = L'\0';
	return s;
}

wchar_t* trim(wchar_t* s)
{
	return rtrim(ltrim(s));
}


DWORD WINAPI CfgParse(LPVOID lpParam)
{
	wchar_t* cfgpath = (wchar_t*)lpParam;
	//MessageBoxW(NULL, cfgpath, L"HELLO", MB_OK);
	FILE* cfg = _wfopen(cfgpath, L"r");
	if (cfg == NULL)
		return 0;

	wchar_t path[MAX_PATH + 1] = { 0 };
	if (fgetws(path, MAX_PATH, cfg) != path)
		return 0;

	wchar_t* wpath = trim(path);

	wchar_t drive[_MAX_DRIVE] = {0};
	wchar_t folder[_MAX_DIR] = { 0 };
	wchar_t fname[_MAX_FNAME] = { 0 };
	wchar_t ext[_MAX_EXT] = { 0 };
	_wsplitpath(wpath, drive, folder, fname, ext);
	wchar_t workingdir[_MAX_DRIVE + _MAX_DIR + 1] = { 0 };
	wcscpy(workingdir, drive);
	wcscat(workingdir, folder);


	LPWSTR cmdlinefull = GetCommandLineW();
	int argc;
	LPWSTR* cmdlines = CommandLineToArgvW(cmdlinefull, &argc);
	wchar_t* wcmdline = (wchar_t*)malloc(sizeof(wchar_t));
	wcscpy(wcmdline, L"");
	for (int i = 1; i < argc; i++)
	{
		LPWSTR cmdline = cmdlines[i];
		LPWSTR localeLocation = wcsstr(cmdline, L"-epiclocale=");
		if (localeLocation != NULL)
		{
			wchar_t locale[4] = { 0 };
			if (fgetws(locale, 3, cfg) == locale)
			{
				wcsncpy(localeLocation + 12, locale, 2);
			}
		}
		wcmdline = (wchar_t*)realloc(wcmdline, (wcslen(wcmdline) + 1 + wcslen(cmdline) + 1) * sizeof(wchar_t));
		wcscat(wcmdline, L" ");
		wcscat(wcmdline, cmdline);
	}
		
	//MessageBoxW(NULL, wcmdline, L"CMDLINE", MB_OK);
	STARTUPINFOW si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	BOOL result = CreateProcessW(wpath, wcmdline, NULL, NULL, FALSE, 0, NULL, workingdir, &si, &pi);
	DWORD err = GetLastError();
	ExitProcess(0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	char path[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		wchar_t curPath[MAX_PATH + 1] = { 0 };
		GetModuleFileNameW(NULL, curPath, MAX_PATH);
		wchar_t drive[_MAX_DRIVE] = { 0 };
		wchar_t folder[_MAX_DIR] = { 0 };
		wchar_t fname[_MAX_FNAME] = { 0 };
		wchar_t ext[_MAX_EXT] = { 0 };
		_wsplitpath(curPath, drive, folder, fname, ext);
		wchar_t cfg[_MAX_DRIVE + _MAX_DIR + 7 + 1 + 1] = { 0 };
		wcscpy(cfg, drive);
		wcscat(cfg, folder);
		wcscat(cfg, L"cfg.txt");
		CopyMemory(path + GetSystemDirectory(path, MAX_PATH - 13), "\\VERSION.dll", 14);
		VERSION.dll = LoadLibrary(path);
		if (VERSION.dll == (HMODULE)false)
		{
			MessageBox(0, "Cannot load original VERSION.dll library", "Proxy", MB_ICONERROR);
			ExitProcess(0);
		}
		VERSION.OrignalGetFileVersionInfoA = GetProcAddress(VERSION.dll, "GetFileVersionInfoA");
		VERSION.OrignalGetFileVersionInfoByHandle = GetProcAddress(VERSION.dll, "GetFileVersionInfoByHandle");
		VERSION.OrignalGetFileVersionInfoExA = GetProcAddress(VERSION.dll, "GetFileVersionInfoExA");
		VERSION.OrignalGetFileVersionInfoExW = GetProcAddress(VERSION.dll, "GetFileVersionInfoExW");
		VERSION.OrignalGetFileVersionInfoSizeA = GetProcAddress(VERSION.dll, "GetFileVersionInfoSizeA");
		VERSION.OrignalGetFileVersionInfoSizeExA = GetProcAddress(VERSION.dll, "GetFileVersionInfoSizeExA");
		VERSION.OrignalGetFileVersionInfoSizeExW = GetProcAddress(VERSION.dll, "GetFileVersionInfoSizeExW");
		VERSION.OrignalGetFileVersionInfoSizeW = GetProcAddress(VERSION.dll, "GetFileVersionInfoSizeW");
		VERSION.OrignalGetFileVersionInfoW = GetProcAddress(VERSION.dll, "GetFileVersionInfoW");
		VERSION.OrignalVerFindFileA = GetProcAddress(VERSION.dll, "VerFindFileA");
		VERSION.OrignalVerFindFileW = GetProcAddress(VERSION.dll, "VerFindFileW");
		VERSION.OrignalVerInstallFileA = GetProcAddress(VERSION.dll, "VerInstallFileA");
		VERSION.OrignalVerInstallFileW = GetProcAddress(VERSION.dll, "VerInstallFileW");
		VERSION.OrignalVerLanguageNameA = GetProcAddress(VERSION.dll, "VerLanguageNameA");
		VERSION.OrignalVerLanguageNameW = GetProcAddress(VERSION.dll, "VerLanguageNameW");
		VERSION.OrignalVerQueryValueA = GetProcAddress(VERSION.dll, "VerQueryValueA");
		VERSION.OrignalVerQueryValueW = GetProcAddress(VERSION.dll, "VerQueryValueW");
		
		DWORD threadId;
		CreateThread(NULL, 0, CfgParse, cfg, 0, &threadId);
		//CfgParse(cfg);

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(VERSION.dll);
	}
	break;
	}
	return TRUE;
}
