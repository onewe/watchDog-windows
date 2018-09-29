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



BOOL InitLog() {
	TCHAR * currentDir = GetFullDir();
	TCHAR * logPath = _tcscat(currentDir,_T(LOG_PATH));
	if (_taccess(logPath, 0) != 0) {
		_ftprintf(stderr,_T("logs日志目录不存在!\n"));
		return FALSE;
	}
	TCHAR * logFilePath = _tcscat(currentDir, _T(LOG_FILE_PATH));
	FILE * log_file = _tfreopen(logFilePath, _T("w+"), stdout);
	if (log_file == NULL) {
		_ftprintf(stderr,_T("打开日志文件失败!程序读取到路径为:%s\n"), logFilePath);
		return FALSE;
	}

	//关闭printf的buff
	setbuf(stdout, NULL);
	//初始化日志
	elog_init();
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO | ELOG_FMT_TAG | ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO | ELOG_FMT_TAG | ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO | ELOG_FMT_TAG | ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	/* start EasyLogger */
	elog_start();
	free(logFilePath);
	return TRUE;
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