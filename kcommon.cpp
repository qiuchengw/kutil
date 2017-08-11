#include "stdafx.h"

#include "kcommon.h"
#include <QMessageBox>
#include <QApplication>

const QString CINFO		= QStringLiteral("提示");
const QString CWARNING	= QStringLiteral("警告");
const QString CERROR    = QStringLiteral("错误");
const QString CCONFIRM  = QStringLiteral("确认");

/*
	显示提示信息
	注意:可变参数中只能使用基本数据类型,否则会出现乱码或异常
*/
void KDLG::info(const char* szMsg, ...)
{
	QString sTempMsg;
	va_list args;
	va_start(args, szMsg);
	sTempMsg.vsprintf(szMsg, args);
	va_end(args);
    QMessageBox::information(qApp->activeWindow(), CINFO, sTempMsg);
}

void KDLG::info(const QString& sMsg)
{
    QMessageBox::information(qApp->activeWindow(), CINFO, sMsg);
}

/*
	显示警告信息
	注意:可变参数中只能使用基本数据类型,否则会出现乱码或异常
*/
void KDLG::warning(const char* szMsg, ...)
{
	QString sTempMsg;
	va_list args;
	va_start(args, szMsg);
	sTempMsg.vsprintf(szMsg, args);
	va_end(args);
    QMessageBox::warning(qApp->activeWindow(), CWARNING, sTempMsg);
}
void KDLG::warning(const QString& sMsg)
{
    QMessageBox::warning(qApp->activeWindow(), CWARNING, sMsg);
}

/*
	显示错误信息
	注意:可变参数中只能使用基本数据类型,否则会出现乱码或异常
*/
void KDLG::error(const char* szMsg, ...)
{
	QString sTempMsg;
	va_list args;
	va_start(args, szMsg);
	sTempMsg.vsprintf(szMsg, args);
	va_end(args);
    QMessageBox::critical(qApp->activeWindow(), CERROR, sTempMsg);
}

void KDLG::error(const QString& sMsg)
{
    QMessageBox::critical(qApp->activeWindow(), CERROR, sMsg);
}

/*
	确认提示信息
	注意:可变参数中只能使用基本数据类型,否则会出现乱码或异常
*/
bool KDLG::confirm(const char* szMsg, ...)
{
	QString sTempMsg;
	va_list args;
	va_start(args, szMsg);
	sTempMsg.vsprintf(szMsg, args);
	va_end(args);
    return QMessageBox::question(qApp->activeWindow(), CCONFIRM, sTempMsg) == QMessageBox::Yes;
}

bool KDLG::confirm(const QString& sMsg)
{
    return QMessageBox::question(qApp->activeWindow(), CCONFIRM, sMsg) == QMessageBox::Yes;
}

