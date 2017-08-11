#ifndef _KUTIL_MISC_H_
#define _KUTIL_MISC_H_

#include "macro.h"
#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>

namespace kutil
{

    // 唯一字符串
    inline QString UuidString()
    {
        // 去掉 ｛｝括号
        return QUuid::createUuid().toString().remove('{').remove('}');
    }

    // 以时间生成可读的编号，适用于调用间隔在1秒以上的使用
    /*
     *	注意！！！
     *      katie工程使用 KatieReadableUniqueString生成唯一编号
     *      不要使用默认参数，这个函数是故意设计的这么难用的
     *      因为不建议直接调用这个函数，而是在各自的库里在此基础上实现自己的函数
     */
    QString ReadableUniqueString(const QString& prefix, const QString& postfix /*= ""*/);

    // 统计文件夹下的文件个数
    void ScanFiles(const QString& dir, const QStringList& filter,
        bool recursive, std::function<bool(const QString& url)> cb);

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
    bool WriteTextFile(const QString& file_path, const QString& content,
        EnumWriteTextMode mode = EnumWriteTextMode::OverWrite, QTextCodec* codec = QTextCodec::codecForName("UTF-8"));

    // 一次性读取所有的内容
    QString ReadTextFile(const QString& file_path, QTextCodec* codec = nullptr);

    // 获取资源
    QString GetResourceFileName(const QString& themeName, const QString& resouce_name);

    // 信号槽连接是否成功
    inline bool CheckedConnect(QObject* sender, const char* signal,
        QObject* reciver, const char* slot, Qt::ConnectionType typ = Qt::AutoConnection)
    {
        Q_ASSERT(nullptr != sender);
        Q_ASSERT(nullptr != reciver);
        Q_ASSERT(nullptr != slot);
        Q_ASSERT(nullptr != signal);

        QMetaObject::Connection cn =
            sender->connect(sender, signal, reciver, slot, typ);
        if (!cn)
        {
            qDebug() << "Signal:" << signal << "\n"
                << "Slot:" << slot;

            Q_ASSERT(false);
            return false;
        }
        return true;
    }

    // 去掉最后n个字符
    inline QString RemoveLast(const QString &s, int n)
    {
        if (s.length() <= n)
            return "";
        QString ret = s;
        return ret.remove(s.length() - n, n);
    }

    // 当前日期
    QString CurrentDate();

    // QVariant的序列化
    QByteArray SaveVariant(const QVariant& v);
    QVariant LoadVariant(const QByteArray& v);

    template <typename _Cont, typename T>
    const T& PrevOrNext(const _Cont& c, const T& current, bool next,
        std::function<bool(const T&)> isFine = [](const T&)->bool { return true;  })
    {
        static T DEF_VAL;

        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (!isFine(*i))
                continue;

            if (*i == current)
            {
                if (!next)
                {
                    // 上一个
                    return (i == c.begin()) ? c.last() : *(--i);
                }
                else
                {
                    // 下一个
                    ++i;
                    return (i == c.end()) ? c.first() : *i;
                }
                break;
            }
        }

        if (c.size() > 0)
        {
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
    QString ReadableNum(double f);

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

    QString url2Filename(const QString& url);

};

#endif // _KUTIL_MISC_H_
