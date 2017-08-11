#ifndef KCOMMON_H
#define KCOMMON_H

#include <QString>

#include <QTimer>

class  KDLG
{
public:
	/*
		显示提示信息
	*/
	static void info(const char* szMsg, ...);
	static void info(const QString& sMsg);

	/*
		显示警告信息
	*/
	static void warning(const char* szMsg, ...);
	static void warning(const QString& sMsg);

	/*
		显示错误信息
	*/
	static void error(const char* szMsg, ...);
	static void error(const QString& sMsg);

	/*
		确认提示框
	*/
    static bool confirm(const char* szMsg, ...);
    static bool confirm(const QString& sMsg);

};

#endif // KCOMMON_H
