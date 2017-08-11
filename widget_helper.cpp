#include "misc.h"

#include <QIcon>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QClipboard>

namespace kutil
{
    namespace widget {
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

                    while (!in.atEnd()) {
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

                    while (!in.atEnd()) {
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

    namespace widget {
        QHash<const QWidget*, QList<TBLFiled>> KTableHelper::table_fields_;
        QHash<const QWidget*, QString> KTableHelper::table_unique_col_;
    }
}

#endif

