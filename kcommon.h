#ifndef KCOMMON_H
#define KCOMMON_H

#include <QString>
#include <QMessageBox>
#include <QApplication>

#define CINFO QStringLiteral("提示")
#define CWARNING QStringLiteral("警告")
#define CERROR QStringLiteral("错误")
#define CCONFIRM QStringLiteral("确认")

class  KDLG
{
public:

	/*
		显示提示信息
	*/
    static void info(const char* szMsg, ...) {
        QString sTempMsg;
        va_list args;
        va_start(args, szMsg);
        sTempMsg.vsprintf(szMsg, args);
        va_end(args);
        QMessageBox::information(qApp->activeWindow(), CINFO, sTempMsg);
    }

    static void info(const QString& sMsg) {
        QMessageBox::information(qApp->activeWindow(), CINFO, sMsg);
    }

	/*
		显示警告信息
	*/
    static void warning(const char* szMsg, ...) {
        QString sTempMsg;
        va_list args;
        va_start(args, szMsg);
        sTempMsg.vsprintf(szMsg, args);
        va_end(args);
        QMessageBox::warning(qApp->activeWindow(), CWARNING, sTempMsg);
    }

    static void warning(const QString& sMsg) {
        QMessageBox::warning(qApp->activeWindow(), CWARNING, sMsg);
    }


	/*
		显示错误信息
	*/
    static void error(const char* szMsg, ...) {
        QString sTempMsg;
        va_list args;
        va_start(args, szMsg);
        sTempMsg.vsprintf(szMsg, args);
        va_end(args);
        QMessageBox::critical(qApp->activeWindow(), CERROR, sTempMsg);
    }

    static void error(const QString& sMsg) {
        QMessageBox::critical(qApp->activeWindow(), CERROR, sMsg);
    }

	/*
		确认提示框
	*/
    static bool confirm(const char* szMsg, ...) {
        QString sTempMsg;
        va_list args;
        va_start(args, szMsg);
        sTempMsg.vsprintf(szMsg, args);
        va_end(args);
        return QMessageBox::question(qApp->activeWindow(), CCONFIRM, sTempMsg) == QMessageBox::Yes;
    }

    static bool confirm(const QString& sMsg) {
        return QMessageBox::question(qApp->activeWindow(), CCONFIRM, sMsg) == QMessageBox::Yes;
    }

};

#undef CINFO // QStringLiteral("提示");
#undef CWARNING // QStringLiteral("警告");
#undef CERROR // QStringLiteral("错误");
#undef CCONFIRM // QStringLiteral("确认");

#endif // KCOMMON_H
