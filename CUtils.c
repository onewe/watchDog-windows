#include "CUtils.h"


//获取程序的完整路径
TCHAR *  GetFullPath() {
	TCHAR *  szModuleFileName = malloc(sizeof(TCHAR) * MAX_PATH + 1);
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	return szModuleFileName;
}
//获取程序当前路径
TCHAR *  GetFullDir() {
	TCHAR * path = GetFullPath();
	char *ch = _tcsrchr(path, _T('\\'));
	*ch = 0;
	return path;
}


wchar_t * CharToWchar(char * target) {
	int len = MultiByteToWideChar(CP_ACP, 0, target, strlen(target), NULL, 0);
	wchar_t * m_wchar = malloc(sizeof(wchar_t) * (len + 1));
	if (m_wchar == NULL) {
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, target, strlen(target), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}


char * WCharToChar(wchar_t * target) {
	int len = WideCharToMultiByte(CP_ACP, 0, target, wcslen(target), NULL, 0, NULL, NULL);
	char * m_char = malloc(sizeof(char) * (len + 1));
	if (m_char == NULL) {
		return NULL;
	}
	WideCharToMultiByte(CP_ACP, 0, target, wcslen(target), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}