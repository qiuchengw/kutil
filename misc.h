#ifndef _KUTIL_MISC_H_
#define _KUTIL_MISC_H_

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>
#include <QDataStream>

namespace kutil
{
    inline QString GBK2UTF8(const QString &inStr){
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        return gbk->toUnicode(gbk->fromUnicode(inStr));              // gbk  convert utf8  
    }

    inline QString UTF82GBK(const QString &inStr){
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

        QString utf2gbk = gbk->toUnicode(inStr.toLocal8Bit());
        return utf2gbk;
    }

    inline std::string gbk2utf8(const QString &inStr){
        return GBK2UTF8(inStr).toStdString();
    }

    inline QString utf82gbk(const std::string &inStr){
        QString str = QString::fromStdString(inStr);
        return UTF82GBK(str);
    }

    // 唯一字符串
    // 去掉 ｛｝括号
    inline QString uuidString() {
        return QUuid::createUuid().toString().remove('{').remove('}');
    }

    // 以时间生成可读的编号，适用于调用间隔在1秒以上的使用
    /*
     *	注意！！！
     *      不要使用默认参数，这个函数是故意设计的这么难用的
     *      因为不建议直接调用这个函数，而是在各自的库里在此基础上实现自己的函数
     */
    inline QString readableUniqueString(const QString& prefix, const QString& postfix /*= ""*/) {
        Q_ASSERT(prefix.length() >= 8); // 

        static int _no = 1; // 递增的
        QDateTime now = QDateTime::currentDateTime();

        QString s = QString("%1%2%3")
            .arg(now.toString("yyyyMMdd"))
            .arg(now.time().msecsSinceStartOfDay() / 1000, 5, 10, QChar('0'))
            .arg(++_no);
        return prefix + s + postfix;
    }

    // 统计文件夹下的文件个数
    inline void scanFiles(const QString& dir, const QStringList& filter,
        bool recursive, std::function<bool(const QString& url)> cb) {
        QDir url(dir);
        if (!url.exists())
            return;

        url.setFilter(QDir::Dirs | QDir::Files);//除了目录或文件，其他的过滤掉
        url.setSorting(QDir::DirsFirst);//优先显示目录

        QFileInfoList list = url.entryInfoList();//获取文件信息列表
        int i = 0;
        do {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.fileName() == "." | fileInfo.fileName() == ".."){
                i++;
                continue;
            }

            if (fileInfo.isDir()) {
                if (recursive) {
                    scanFiles(fileInfo.filePath(), filter, recursive, cb);
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

    /**
    *	写文本文件
    *      ！！！ 支持格式设置
    *
    **/
    enum EnumWriteTextMode {
        WriteAppend,
        WritePreAppend,
        OverWrite,
    };


    // 一次性读取所有的内容
    inline QString readTextFile(const QString& file_path, QTextCodec* codec = nullptr) {
        QString s_all;
        QFile file(file_path);
        if (file.open(QFile::ReadOnly)) {
            QTextStream ts(&file);
            if (nullptr == codec) {
                codec = QTextCodec::codecForName("GBK");
            }
            ts.setCodec(codec);
            s_all = ts.readAll();
            file.close();
            return s_all;
        }
        return QString("");
    }

    inline bool writeTextFile(const QString& file_path, const QString& content,
        EnumWriteTextMode mode = EnumWriteTextMode::OverWrite, QTextCodec* codec = QTextCodec::codecForName("UTF-8")) {
        QString old_content;
        if (mode != EnumWriteTextMode::OverWrite) {
            old_content = kutil::readTextFile(file_path, codec);
        }

        QFile::remove(file_path);
        QFile file(file_path);
        if (file.open(QFile::ReadWrite)){
            QTextStream ts(&file);
            if (nullptr != codec){
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

    // 信号槽连接是否成功
    inline bool checkedConnect(QObject* sender, const char* signal,
        QObject* reciver, const char* slot, Qt::ConnectionType typ = Qt::AutoConnection){
        Q_ASSERT(nullptr != sender);
        Q_ASSERT(nullptr != reciver);
        Q_ASSERT(nullptr != slot);
        Q_ASSERT(nullptr != signal);

        if (!sender->connect(sender, signal, reciver, slot, typ)){
            qDebug() << "Signal:" << signal << "\n"<< "Slot:" << slot;
            Q_ASSERT(false);
            return false;
        }
        return true;
    }

    // 去掉最后n个字符
    inline QString removeLast(const QString &s, int n){
        if (s.length() <= n)
            return "";
        QString ret = s;
        return ret.remove(s.length() - n, n);
    }

    // 当前日期
    inline QString currentDate() {
        return QDate::currentDate().toString("yyyyMMdd");
    }

    // QVariant的序列化
    inline QByteArray saveVariant(const QVariant& v) {
        QByteArray ret;
        QDataStream ds(&ret, QIODevice::ReadWrite);
        v.save(ds);
        return ret;
    }
    
    inline QVariant loadVariant(const QByteArray& v) {
        QDataStream ds(v);
        QVariant ret;
        ret.load(ds);
        return ret;
    }

    template <typename _Cont, typename T>
    const T& prevOrNext(const _Cont& c, const T& current, bool next,
        std::function<bool(const T&)> isFine = [](const T&)->bool { return true;  })
    {
        static T DEF_VAL;

        for (auto i = c.begin(); i != c.end(); ++i){
            if (!isFine(*i))
                continue;

            if (*i == current){
                if (!next){
                    // 上一个
                    return (i == c.begin()) ? c.last() : *(--i);
                }
                else{
                    // 下一个
                    ++i;
                    return (i == c.end()) ? c.first() : *i;
                }
                break;
            }
        }

        if (c.size() > 0){
            // 默认返回第一个
            return c.front();
        }

        // 如果是空容器/或者未找到
        Q_ASSERT(false);
        return DEF_VAL;
    }

    // 大于万的精确到小数点后两位，小于万的去掉小数点
    // 12.30  -> 12  (去掉小数点)
    // 123000  ->  12.30 万
    // 1,2300,0000  ->  1.23 亿
    inline QString readableNum(double f) {
        auto my_round = [](double x) {
            Q_ASSERT(x <= 10000000); // 防止溢出
            return (long)(x * 100) / 100.f;
        };

        const double yi = 10000 * 10000;    // 一亿
        if (f >= yi){
            return QString::number(my_round(f / yi), 'f', 2) + QStringLiteral("亿");
        }

        const double wan = 10000;
        if (f >= wan){
            return QString::number(my_round(f / wan), L'f', 2) + QStringLiteral("万");
        }
        // 不要小数点
        return QString::number(f, 'f', 2);
    }

    inline QString normalFilename(const QString& f) {
        QString name = f;
        return name.replace(QRegExp("[/*?:<>|=&;\"\\\\]"), "")
            .remove("+").remove("-").remove("/").remove(".");
    }

    inline QString md5Name(const QByteArray& cont) {
        QString str = QCryptographicHash::hash(cont, QCryptographicHash::Md5)
            .toBase64(QByteArray::OmitTrailingEquals);
        return normalFilename(str);
    }

    inline QString url2Filename(const QString& url) {
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

#endif // _KUTIL_MISC_H_
