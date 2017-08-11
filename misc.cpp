#include "stdafx.h"
#include "misc.h"
#include <QTextStream>
#include <QFile>
#include <QPoint>
#include <QDate>
#include <QDateTime>
#include <QDataStream>
#include <QTextCodec>
#include <QDir>

namespace qui
{
    QString ReadableUniqueString(const QString& prefix, const QString& postfix)
    {
        Q_ASSERT(prefix.length() >= 8); // 

        static int _no = 1; // 递增的
        QDateTime now = QDateTime::currentDateTime();

        QString s = QString("%1%2%3")
            .arg(now.toString("yyyyMMdd"))
            .arg(now.time().msecsSinceStartOfDay() / 1000, 5, 10, QChar('0'))
            .arg(++_no);
        return prefix + s + postfix;
    }

    bool WriteTextFile(const QString& file_path, const QString& content, 
        EnumWriteTextMode mode /*= EnumWriteTextMode::OverWrite*/, QTextCodec* codec /*= nullptr*/)
    {
        QString old_content;
        if (mode != EnumWriteTextMode::OverWrite){
            old_content = qui::ReadTextFile(file_path, codec);
        }
        QFile::remove(file_path);

        QFile file(file_path);
        if (file.open(QFile::ReadWrite))
        // if (file.open(QFile::Append | QFile::Text))
        {
            QTextStream ts(&file);
            if (nullptr != codec)
            {
                ts.setCodec(codec);
                ts.setGenerateByteOrderMark(true);
            }

            switch (mode)
            {
            case EnumWriteTextMode::OverWrite: ts << content; break;
            case EnumWriteTextMode::WriteAppend: ts << old_content; ts << content; break;
            case EnumWriteTextMode::WritePreAppend: ts << content; ts << old_content; break;
            }
            return true;
        }
        return false;
    }

    QString ReadTextFile(const QString& file_path, QTextCodec *codec)
    {
        QString s_all;
        QFile file(file_path);
        if (file.open(QFile::ReadOnly))
        {
            QTextStream ts(&file);
            if (nullptr == codec)
            {
                codec = QTextCodec::codecForName("GBK");
            }
            ts.setCodec(codec);
            s_all = ts.readAll();
            file.close();
            return s_all;
        }
        return QString("");
    }

    QString GetResourceFileName(const QString& themeName, const QString& resouce_name)
    {
        Q_UNUSED(themeName);
        Q_UNUSED(resouce_name);

        return "";
    }

    void ScanFiles(const QString& dir, const QStringList& filter, bool recursive,  std::function<bool(const QString& url)> cb)
    {
        QDir url(dir);
        if (!url.exists())
            return;

        url.setFilter(QDir::Dirs | QDir::Files);//除了目录或文件，其他的过滤掉
        url.setSorting(QDir::DirsFirst);//优先显示目录

        QFileInfoList list = url.entryInfoList();//获取文件信息列表
        int i = 0;
        do {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
            {
                i++;
                continue;
            }

            if (fileInfo.isDir()) {
                if (recursive) {
                    ScanFiles(fileInfo.filePath(), filter, recursive, cb);
                }
            }
            else if (fileInfo.isFile()) {
                QString suffix = fileInfo.suffix();
                if (cb && filter.contains(suffix, Qt::CaseInsensitive)) {
                    if (!cb(fileInfo.filePath())) {
                        break;
                    }
                }
            }
            i++;
        } while (i < list.size());
    }


//     bool UpgradeProcessPrivilege()
//     {
//         HANDLE hToken;
//         TOKEN_PRIVILEGES tkp;
// 
//         // Get a token for this process.
//         if (!OpenProcessToken(GetCurrentProcess(),
//             TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
//             return false;
// 
//         // Get the LUID for the shutdown privilege.
//         LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
//             &tkp.Privileges[0].Luid);
// 
//         tkp.PrivilegeCount = 1; // one privilege to set 
//         tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
// 
//         // Get the shutdown privilege for this process.
//         AdjustTokenPrivileges(hToken, false, &tkp, 0,
//             (PTOKEN_PRIVILEGES)NULL, 0);
// 
//         if (GetLastError() != ERROR_SUCCESS)
//             return false;
// 
//         return true;
//     }

    QString CurrentDate()
    {
        return QDate::currentDate().toString("yyyyMMdd");
    }

    QByteArray SaveVariant(const QVariant& v)
    {
        QByteArray ret;
        QDataStream ds(&ret, QIODevice::ReadWrite);
        v.save(ds);
        return ret;
    }

    QVariant LoadVariant(const QByteArray& v)
    {
        QDataStream ds(v);
        QVariant ret;
        ret.load(ds);
        return ret;
    }

    QString ReadableNum(double f)
    {
        auto my_round = [](double x){
            Q_ASSERT(x <= 10000000); // 防止溢出
            return (long)(x * 100) / 100.f;
        };

        const double yi = 10000 * 10000;    // 一亿
        if (f >= yi)
        {
            return QString::number(my_round(f / yi), 'f', 2) + QStringLiteral("亿");
        }

        const double wan = 10000;
        if (f >= wan)   // 一万
        {
            return QString::number(my_round(f / wan), L'f', 2) + QStringLiteral("万");
        }

        // 不要小数点
        return QString::number(f, 'f', 2);
    }
};

//////////////////////////////////////////////////////////////////////////

#ifdef QT_WIDGETS_LIB

#include <QIcon>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QClipboard>

namespace qui
{
    QRect windowRect(QWidget *w)
    {
        QRect r = w->rect();
        r.moveTo(w->mapToGlobal(r.topLeft()));
        return r;
    }

    QPoint GetWidgetScreenPos(QWidget* widget)
    {
        return windowRect(widget).topLeft();
    }

    QIcon GetResourceIcon(const QString& themeName, const QString& resouce_name)
    {
        return QIcon(themeName + resouce_name);
    }

    bool ApplyStyleSheet(QWidget* wdt, const QString& res)
    {
        Q_ASSERT(wdt != nullptr);
        Q_ASSERT(!res.isEmpty());

        if (!QFile::exists(res))
            return false;

        QFile file(res);
        if (file.open(QFile::ReadOnly))
        {
            if (file.isOpen())
            {
                wdt->setStyleSheet(QTextStream(&file).readAll());

                return true;
            }
        }
        return false;
    }

    void CenterWidget(QWidget* w, QPoint pt)
    {
        if (nullptr == w)
        {
            Q_ASSERT(false);
            return;
        }

        QRect rc = w->rect();
        rc.moveCenter(pt);
        w->move(rc.topLeft());
    }

    void CenterWidgetDesktop(QWidget* w)
    {
        CenterWidget(w, QApplication::desktop()->availableGeometry().center());
    }

    void MoveToFullWorkArea(QWidget* wdt)
    {
        if (nullptr != wdt)
        {
            wdt->setGeometry(QApplication::desktop()->availableGeometry());
        }
    }

    void SaveToCSV(QStandardItemModel* m)
    {
        if (m->rowCount() != 0)
        {
            QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("打开文件"),
                "", "*.csv", 0, QFileDialog::DontUseNativeDialog);
            QString file = filepath + ".csv";

            //添加写csv数据表的内容
            QList<QString> str_list_;
            //读取标题
            QString str_head_;
            for (int i = 0; i < m->columnCount(); i++)
            {
                QStandardItem* iitem = m->horizontalHeaderItem(i);
                if (i == m->columnCount() - 1)
                {
                    str_head_.append(iitem->text());
                    str_head_.append("\n");
                }
                else
                {
                    str_head_.append(iitem->text());
                    str_head_.append(",");
                }

            }

            str_list_.push_back(str_head_);
            //读取内容
            for (int i = 0; i < m->rowCount(); i++)
            {
                QString str;
                for (int j = 0; j < m->columnCount(); j++)
                {
                    QStandardItem* iitem = m->item(i, j);
                    if (j == m->columnCount() - 1)
                    {
                        str.append(iitem->text());
                        str.append("\n");
                    }
                    else
                    {
                        str.append(iitem->text());
                        str.append(",");
                    }

                }
                // // qDebug() << str_;
                str_list_.push_back(str);

            }

            QFile csvFile(file);
            if (csvFile.open(QIODevice::ReadWrite))
            {
                QTextStream out(&csvFile);
                //出现换行错位现象，因为字符串中含有 \r,\n字符
                for (int i = 0; i < str_list_.count(); i++)
                {
                    QString str_temp = str_list_.at(i);
                    if (str_temp.contains("\r") || str_temp.contains("\n"))
                    {
                        str_temp.remove("\r", Qt::CaseInsensitive).remove("\n", Qt::CaseInsensitive);
                    }
                    out << str_temp << "\n";
                }
            }
        }
    }

	void SaveToCSV(QTableWidget* pcurtable)
	{
        if (0 == pcurtable->rowCount())
            return;

		QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("打开文件"),
			"", "*.csv", 0, QFileDialog::DontUseNativeDialog);
		if (filepath.isEmpty())
		{
			return;
		}
		QString file = filepath + ".csv";

		//添加写csv数据表的内容
		QList<QString> str_list_;
		for (int i = -1; i < pcurtable->rowCount(); i++)
		{
			QString str;
			for (int j = 0; j < pcurtable->columnCount(); j++)
			{
				if (-1 == i)
				{
					//标题
					str.append(pcurtable->horizontalHeaderItem(j)->text());
				}
				else
				{
					//内容
					str.append(pcurtable->item(i, j)->text());
				}

				j == (pcurtable->columnCount() - 1) ?
					str.append("\n") : str.append(",");
			}
			str_list_.push_back(str);
		}

		QFile csvFile(file);
		if (csvFile.open(QIODevice::ReadWrite))
		{
			QTextStream out(&csvFile);
			//出现换行错位现象，因为字符串中含有 \r,\n字符
			for (int i = 0; i < str_list_.count(); i++)
			{
				QString str_temp = str_list_.at(i);
				str_temp.replace("\r\n", "");
				str_temp.replace("\r", "");
				str_temp.replace("\n", "");
				out << str_temp << "\n";
			}
		}
	}

#ifdef KColoredTree_h_
	void SaveToCSV(KColoredTree* theTree)
	{
		if (0 >= theTree->getMainRowCount())
			return;
		QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("打开文件"),
			"", "*.csv", 0, QFileDialog::DontUseNativeDialog);
		if (filepath.isEmpty())
		{
			return;
		}
		QString file = filepath + ".csv";

		//添加写csv数据表的内容
		QList<QStringList> str_list_;
		QStringList pos_headlist = theTree->getHeaderLabel();
		str_list_.push_back(pos_headlist);
		for (int i = 0; i < theTree->getMainRowCount(); i++)
		{
			//获取母单信息
			QStringList maintext;
			//存入str_list_中
			QList<int> mainRowList;
			mainRowList.append(i);
			maintext = theTree->getChildTreeItem(mainRowList);
			if (!maintext.isEmpty())
			{
				str_list_.push_back(maintext);
			}
			for (int j = 0; j < theTree->getChildRowCount(i); j++)
			{
				//获取子单信息
				QStringList childtext;
				//存入str_list_中
				QList<int> childRowList;
				childRowList.append(i);
				childRowList.append(j);
				childtext = theTree->getChildTreeItem(childRowList);
				if (!childtext.isEmpty())
				{
					str_list_.push_back(childtext);
				}

			}

		}
		QFile csvFile(file);
		if (csvFile.open(QIODevice::ReadWrite))
		{
			QTextStream out(&csvFile);
			//出现换行错位现象，因为字符串中含有 \r,\n字符
			for (int i = 0; i < str_list_.count(); i++)
			{
				QString str_temp = str_list_.at(i).join(",");
				str_temp.replace("\r\n", "");
				str_temp.replace("\r", "");
				str_temp.replace("\n", "");
				out << str_temp << "\n";
			}
		}
	}
#endif

	bool SaveToCSV(QList<QStringList>& content)
	{
		QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("打开文件"),
			"", "*.csv", 0, QFileDialog::DontUseNativeDialog);
		if (filepath.isEmpty())
		{
			return false;
		}
		QString file = filepath + ".csv";
		QFile csvFile(file);
		if (csvFile.open(QIODevice::ReadWrite))
		{
			QTextStream out(&csvFile);
			//出现换行错位现象，因为字符串中含有 \r,\n字符
			for (int i = 0; i < content.count(); i++)
			{
				QString str_temp = content.at(i).join(",");
				str_temp.replace("\r\n", "");
				str_temp.replace("\r", "");
				str_temp.replace("\n", "");
				out << str_temp << "\n";
			}
		}
        return true;
	}

	QList<QStringList> ReadFromCVS(QTextCodec *codec)
	{
		QList<QStringList> returnList;
		auto filelist = QFileDialog::getOpenFileUrls(nullptr, QStringLiteral("打开文件"),
			QUrl(), "*.csv", 0, QFileDialog::DontUseNativeDialog);

		for (auto url : filelist)
		{
			QString file = url.toLocalFile();
			QFile csvFile(file);
			if (csvFile.open(QIODevice::ReadWrite))
			{
				if (nullptr == codec)
					codec = QTextCodec::codecForName("GBK");
				QTextStream in(&csvFile);
				in.setCodec(codec);

				while (!in.atEnd()){
					QString line = in.readLine();
					returnList.push_back(line.split(","));
				}
			}
		}
		return returnList;
	}

    bool ReadFromCVS(QList<QStringList>& ctn, QTextCodec *codec)
    {
        QList<QStringList> returnList;
        auto filelist = QFileDialog::getOpenFileUrls(nullptr, QStringLiteral("打开文件"),
            QUrl(), "*.csv", 0, QFileDialog::DontUseNativeDialog);

        if (filelist.size() == 0)
            return false;

        for (auto url : filelist)
        {
            QString file = url.toLocalFile();
            QFile csvFile(file);
            if (csvFile.open(QIODevice::ReadWrite))
            {
                if (nullptr == codec)
                    codec = QTextCodec::codecForName("GBK");
                QTextStream in(&csvFile);
                in.setCodec(codec);

                while (!in.atEnd()){
                    QString line = in.readLine();
                    ctn.push_back(line.split(","));
                }
            }
        }
        return true;
    }

	void CopyTableWidget2Clipboard(QTableWidget* pcurtable)
	{
		if (0 != pcurtable->rowCount())
		{
			if (-1 != pcurtable->currentIndex().row())
			{
				QString str;
				for (int i = 0; i < pcurtable->columnCount(); i++)
				{
					str.append(pcurtable->item(pcurtable->currentIndex().row(), i)->text());
					str.append("  ");
				}

				QClipboard *board = QApplication::clipboard();
				board->setText(str);
			}
		}
	}

    void CopyTableModel2Clipboard(QStandardItemModel* m)
    {
        int row = m->rowCount();
        int col = m->columnCount();
        if (row <= 0)
        {
            return;
        }
        QString str_copy;
        //复制头标题
        for (int k = 0; k < col; k++)
        {
            str_copy.append(m->horizontalHeaderItem(k)->text());
            if (k == col - 1)
            {
                str_copy.append("\n");
            }
            else
            {
                str_copy.append(",");
            }
        }
        //复制内容
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                str_copy.append(m->item(i, j)->text());
                if (j == col - 1)
                {
                    str_copy.append("\n");
                }
                else
                {
                    str_copy.append(",");
                }
            }
        }
        QClipboard *board = QApplication::clipboard();
        board->setText(str_copy);
    }

    void ClearTable(QAbstractItemModel* m)
    {
        for (int i = m->rowCount(); i >= 0; i--)
        {
            m->removeRow(i);
        }
    }

    bool IsInFocusingWindow(QWidget* w)
    {
        if (nullptr != w)
        {
            if (QWidget* focus_w = QApplication::focusWidget())
            {
                if ((w == focus_w) || w->isAncestorOf(focus_w))
                    return true;

                return focus_w->window()->isAncestorOf(w);
            }
        }
        return false;
    }

	bool IsFloatZero(const float& number)
	{
		if (fabs(number) <= 10e-6)
		{
			return true;
		}

		return false;
	}

	bool IsDoubleZero(const double& number)
	{
		if (fabs(number) <= 10e-6)
		{
			return true;
		}

		return false;
	}

	//void ShowProfitLoss(const QString& stockid,const QString& strTips, 
	//					const QString& strContext,bool bNewPos)
	//{
//        Q_ASSERT(false);
// 		if (KHomeMsgDlg::homemsgdlg == nullptr)
// 		{
// 			KHomeMsgDlg::homemsgdlg = new KHomeMsgDlg(stockid, strTips, strContext);
// 			KHomeMsgDlg::homemsgdlg->show();
// 
// 			if (bNewPos)
// 			{
// 				KHomeMsgDlg::homemsgdlg->move(KHomeMsgDlg::homemsgdlg->x(),
// 					KHomeMsgDlg::homemsgdlg->y() - 220);
// 			}
// 		}
// 		else
// 		{
// 			KHomeMsgDlg::homemsgdlg->SetDlgInfo(strTips, strContext);
// 		}
	//}

    bool indexVisibleItem(QAbstractItemView* v, __p_out QModelIndex& idxFirst, __p_out QModelIndex& idxLast)
    {
        if (nullptr == v)
        {
            Q_ASSERT(false);
            return false;
        }

        if (QAbstractItemModel* m = v->model())
        {
            QRect rc = v->rect();
            idxFirst = v->indexAt(rc.topLeft());
            if (!idxFirst.isValid())
                return false;

            idxLast = v->indexAt(rc.bottomLeft());
            if (!idxLast.isValid())
            {
                idxLast = idxFirst.sibling(m->rowCount() - 1, idxFirst.column());
            }
            return true;
        }
        return true;
    }
}

QHash<const QWidget*, QList<TBLFiled>> KTableHelper::table_fields_;
QHash<const QWidget*, QString> KTableHelper::table_unique_col_;

#endif

