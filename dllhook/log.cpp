#include "log.h"

LogFile::LogFile(const char* strFileName)
	: logFileName(strFileName)
{
	::fopen_s(&m_fp, logFileName, "w");
}

LogFile::~LogFile()
{
	if (m_fp) {
		::fclose(m_fp);
	}
}

void LogFile::Write(const char *str)
{
	size_t len = strlen(str);
	fwrite(str, 1, len, m_fp);
	::fflush(m_fp);
}

static LogFile g_log_file("log.txt");

LogUtils* LogUtils::Instance()
{
	static LogUtils sLogUtils;
	return &sLogUtils;
}

void LogUtils::LogInfo(const char *fmt, ...)
{
	char s[1024] = "";
	va_list argList;
	va_start(argList, fmt);
	vsnprintf(s, 1024, fmt, argList);
	va_end(argList);

	size_t len = strlen(s);
//	assert(len < 1022);
	s[len++] = '\n';
	s[len] = '\0';

	g_log_file.Write(s);
	printf(s);

}
