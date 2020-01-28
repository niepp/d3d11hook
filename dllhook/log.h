#pragma once
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

class LogFile
{
public:
	LogFile(const char* strFileName);
	~LogFile();

	void Write(const char *str);

private:
	const char* logFileName;
	FILE *m_fp;
};

class LogUtils {
public:
	static LogUtils* Instance();
	void LogInfo(const char *fmt, ...);

};

