#include "misc.h"
#include <QTextStream>
#include <QFile>
#include <QPoint>
#include <QDate>
#include <QDateTime>
#include <QDataStream>
#include <QTextCodec>
#include <QDir>

namespace kutil
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
            old_content = kutil::ReadTextFile(file_path, codec);
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

    QString url2Filename(const QString& url) {
        // http://xxx/image.php?id=4FBB58096F38&amp;jpg
        // 以上是一个图片的url，这种情况下就不能使用QUrl::name来获取名字
        QString name;
        int idx = url.lastIndexOf("/");
        if (-1 != idx) {
            // 找到最后一个 / ，其后的应该都可以算名字了
            name = url.mid(idx + 1);
        }
        else {
            name = url;
        }

        // 但是要移除掉一些文件系统中不能使用的非法字符
        name = normalFilename(name);
        if (!name.isEmpty()) {
            return name;
        }
        return "noname";
    }
};
