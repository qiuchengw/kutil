#ifndef _KUTIL_MISC_H_
#define _KUTIL_MISC_H_

#include <functional>

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include <QDir>
#include <QTemporaryFile>
#include <QDateTime>
#include <QCryptographicHash>

namespace kutil
{
    inline QString GBK2UTF8(const QString &inStr){
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        // QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        return gbk->toUnicode(gbk->fromUnicode(inStr));              // gbk  convert utf8  
    }

    inline QString UTF82GBK(const QString &inStr){
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        // QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

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

    // Ψһ�ַ���
    // ȥ�� ��������
    inline QString uuidString() {
        return QUuid::createUuid().toString().remove('{').remove('}');
    }

    // ��ʱ�����ɿɶ��ı�ţ������ڵ��ü����1�����ϵ�ʹ��
    /*
     *	ע�⣡����
     *      ��Ҫʹ��Ĭ�ϲ�������������ǹ�����Ƶ���ô���õ�
     *      ��Ϊ������ֱ�ӵ�����������������ڸ��ԵĿ����ڴ˻�����ʵ���Լ��ĺ���
     */
    inline QString readableUniqueString(const QString& prefix, const QString& postfix /*= ""*/) {
        Q_ASSERT(prefix.length() >= 8); // 

        static int _no = 1; // ������
        QDateTime now = QDateTime::currentDateTime();

        QString s = QString("%1%2%3")
            .arg(now.toString("yyyyMMdd"))
            .arg(now.time().msecsSinceStartOfDay() / 1000, 5, 10, QChar('0'))
            .arg(++_no);
        return prefix + s + postfix;
    }

    // ͳ���ļ����µ��ļ�����
    inline void scanFiles(const QString& dir, const QStringList& filter,
        bool recursive, std::function<bool(const QString& url)> cb) {
        QDir url(dir);
        if (!url.exists())
            return;

        url.setFilter(QDir::Dirs | QDir::Files);//����Ŀ¼���ļ��������Ĺ��˵�
        url.setSorting(QDir::DirsFirst);//������ʾĿ¼

        QFileInfoList list = url.entryInfoList();//��ȡ�ļ���Ϣ�б�
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
    *	д�ı��ļ�
    *      ������ ֧�ָ�ʽ����
    *
    **/
    enum EnumWriteTextMode {
        WriteAppend,
        WritePreAppend,
        OverWrite,
    };


    // һ���Զ�ȡ���е�����
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

    // �źŲ������Ƿ�ɹ�
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

    // ȥ�����n���ַ�
    inline QString removeLast(const QString &s, int n){
        if (s.length() <= n)
            return "";
        QString ret = s;
        return ret.remove(s.length() - n, n);
    }

    // ��ǰ����
    inline QString currentDate() {
        return QDate::currentDate().toString("yyyyMMdd");
    }

    // QVariant�����л�
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
                    // ��һ��
                    return (i == c.begin()) ? c.last() : *(--i);
                }
                else{
                    // ��һ��
                    ++i;
                    return (i == c.end()) ? c.first() : *i;
                }
                break;
            }
        }

        if (c.size() > 0){
            // Ĭ�Ϸ��ص�һ��
            return c.front();
        }

        // ����ǿ�����/����δ�ҵ�
        Q_ASSERT(false);
        return DEF_VAL;
    }

    // ������ľ�ȷ��С�������λ��С�����ȥ��С����
    // 12.30  -> 12  (ȥ��С����)
    // 123000  ->  12.30 ��
    // 1,2300,0000  ->  1.23 ��
    inline QString readableNum(double f) {
        auto my_round = [](double x) {
            Q_ASSERT(x <= 10000000); // ��ֹ���
            return (long)(x * 100) / 100.f;
        };

        const double yi = 10000 * 10000;    // һ��
        if (f >= yi){
            return QString::number(my_round(f / yi), 'f', 2) + QStringLiteral("��");
        }

        const double wan = 10000;
        if (f >= wan){
            return QString::number(my_round(f / wan), L'f', 2) + QStringLiteral("��");
        }
        // ��ҪС����
        return QString::number(f, 'f', 2);
    }

    inline QString normalFilename(const QString& f) {
        QString name = f;
        return name.replace(QRegExp("[/*?:<>|=&;\"\\\\]"), "")
            .remove("+").remove("-").remove("/");
    }

    inline QString md5Name(const QByteArray& cont) {
        QString str = QCryptographicHash::hash(cont, QCryptographicHash::Md5)
            .toBase64(QByteArray::OmitTrailingEquals);
        return normalFilename(str);
    }

    inline QString url2Filename(const QString& url) {
        // http://xxx/image.php?id=4FBB58096F38&amp;jpg
        // ������һ��ͼƬ��url����������¾Ͳ���ʹ��QUrl::name����ȡ����
        QString name;
        int idx = url.lastIndexOf("/");
        if (-1 != idx) {
            // �ҵ����һ�� / ������Ӧ�ö�������������
            name = url.mid(idx + 1);
        }
        else {
            name = url;
        }

        // ����Ҫ�Ƴ���һЩ�ļ�ϵͳ�в���ʹ�õķǷ��ַ�
        name = normalFilename(name);
        if (!name.isEmpty()) {
            return name;
        }
        return "noname";
    }

	// ��ȡ�ļ����ݣ�����sha1��ת��Ϊbase64���룬����Ϊ�ļ����洢
	// ���Ƶ�dir�ļ�����
	inline QString copy2Sha1FileName(const QString &file, const QString& dir){
		QFile fi(file); // �п���Ȩ�����⣬�����޷���
		if (fi.open(QFile::ReadOnly)) {
			QByteArray cont_sha1 = QCryptographicHash::hash(fi.readAll(), QCryptographicHash::Sha1);
			QString cache_file = dir + "/" + md5Name(cont_sha1);
			if (!QFileInfo::exists(cache_file)) {
				QFile::copy(file, cache_file);
			}
			return cache_file;
		}
		return QString::null;
	}

	// һ�������tempfile �ļ�������֤�ļ�һ�������ڣ�
	// ���ʵ�ֺ�������ΪҪ����/ɾ�������ļ���
	inline QString randomTempFileName() {
		QTemporaryFile file;
		// ֻҪ�ļ������ļ�Ҫɾ��
		file.setAutoRemove(true);
		if (file.open()) {
			return file.fileName();
		}
		return QString::null;
	}

	// �����ļ���ϵͳtemp�ļ����£����ر��ݵ��ļ�ȫ·��
	inline QString backupFile(const QString &origin) {
		QString name = randomTempFileName();
		if (QFile::copy(origin, name)) {
			return name;
		}
		return QString::null;
	}
};

#endif // _KUTIL_MISC_H_
