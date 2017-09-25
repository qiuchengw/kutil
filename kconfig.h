#ifndef kconfig_h__
#define kconfig_h__

#include "rjson.h"

#ifndef QT_DLL
#define QT_DLL
#endif

#ifdef QT_DLL

#include <QFile>
#include "misc.h"
typedef QString String;
#define _utf8_str(val) RJsonValue(val.toUtf8().constData(), cfg_->GetAlloctor())
#define _utf8_p(val) RJsonValue(val, cfg_->GetAlloctor())

#else   // c++ pure

// ʹ��std::string
#ifndef RAPIDJSON_HAS_STDSTRING
#   define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <vector>
#include <string>
typedef std::string String;
#define _utf8_str(val) RJsonValue(val.c_str(), cfg_->GetAlloctor())
#define _utf8_p(val) RJsonValue(val, cfg_->GetAlloctor())

#endif  // QT_DLL


class IConfig
{
public:
    // pretty : true ����ʽ�������к������������������ݰ�
    virtual String GetConfig(bool pretty = true)const = 0;
};

class KConfig;
class KConfigValue : public IConfig
{
    friend class KConfig;

    KConfigValue(const KConfigValue &) = delete;
    KConfigValue &operator=(const KConfigValue &) = delete;

protected:
    KConfigValue(rapidjson::Type typ, KConfig* c)
    {
        assert(nullptr != c);

        jv_ = new RJsonValue(typ);
        cfg_ = c;
    }

    inline KConfigValue(RJsonValue *v, KConfig* c);

    KConfigValue()
    {
        jv_ = nullptr;
        cfg_ = nullptr;
    }

    virtual ~KConfigValue()
    {
        if (nullptr != jv_)
        {
            delete jv_;
        }

        for (auto *p : values_)
        {
            delete p;
        }
    }

    inline RJsonValue* jval()const
    {
        return jv_;
    }

public:
    // �������͵�ֵʹ��
    inline KConfigValue& PushBack(KConfigValue* v);

    inline KConfigValue& PushBack(const String& v);

    template <class _typ>
    KConfigValue& PushBack(_typ val);

    virtual String GetConfig(bool pretty = true)const override
    {
        String s_conf;
        if (rapidjson::SaveToString(jv_, s_conf, pretty, 2))
        {
            return s_conf;
        }
        return "";
    }

#ifdef QT_DLL
    inline QByteArray GetConfigUtf8() const
    {
        return GetConfig().toUtf8();
    }

    bool WriteToFile(const String& file_path/*, bool encode_base64 = false*/)
    {
        String s_conf;
        if (rapidjson::SaveToString(jv_, s_conf, true, 2))
        {
            return kutil::writeTextFile(file_path, s_conf);
        }
        return false;
    }
#endif

    /**
    *	�ڱ������õĹ����л�����value
    *      ��Ҫͨ���˷�������
    *
    **/
    inline KConfigValue* NewValue(rapidjson::Type typ)
    {
        KConfigValue *v = new KConfigValue(typ, this->cfg_);
        values_.push_back(v);
        return v;
    }

    inline KConfigValue* nodeValue(const QString& node_path);    // path/to/node

    inline KConfig* NewConfig(const QString& json);

    inline KConfigValue& AddMember(const String& name, const char* val);

    inline KConfigValue& AddMember(const String& name, KConfigValue* val);
    
    inline KConfigValue& AddMember(const String& name, const String& val);

    inline KConfigValue& AddMember(const String& name, KConfig* cfg)
    {
        return AddMember(name, (KConfigValue*)cfg);
    }

    template <class _typ>
    KConfigValue& AddMember(const String& name, _typ val)
    {
        static_assert(!std::is_pointer<_typ>::value, "can't be pointer type!");

        // _typ ������value��config
        // GCC/ mingw ��Ҫ����һ����ʱ����
        RJsonValue _v(val);
        return _AddMember_internal(name, _v);
    }

protected:
    inline bool Exist(KConfigValue* val) const
    {
        return std::find(values_.begin(), values_.end(), val) != values_.end();
    }

    inline KConfigValue& _AddMember_internal(const String& name, RJsonValue& v);
protected:
    // real value
    RJsonValue*   jv_ = nullptr;

    // �������е�value
    std::vector<KConfigValue*> values_;

private:
    // �ĵ�
    KConfig* cfg_ = nullptr;
};

// ��Ӧ��rapidjson����document
class KConfig : public KConfigValue
{
    typedef RJsonDocument::AllocatorType Alloctor;
    
    KConfig(const KConfig &) = delete; 
    KConfig &operator=(const KConfig &) = delete;

public:
    KConfig(const String& d = "{}")
    {
        jv_ = new RJsonDocument;
        cfg_ = this;

#ifdef QT_DLL
        doc()->Parse<0>(d.toUtf8().constData());
#else
        doc()->Parse<0>(d.c_str());
#endif
    }

    virtual ~KConfig()
    {
        delete doc();

        jv_ = nullptr;
    }

    inline RJsonDocument* doc()const
    {
        return (RJsonDocument*)(jv_);
    }

    inline bool hasError()const
    {
        return doc()->HasParseError();
    }

public:
    inline Alloctor& GetAlloctor()
    {
        return doc()->GetAllocator();
    }

#ifdef QT_DLL 
    bool ReadFromFile(const String& file_path)
    {
        QFile file(file_path);
        if (file.open(QFile::ReadOnly))
        {
            String all = file.readAll();
            doc()->Parse<0>(all.toUtf8().constData());
            if (doc()->HasParseError())
            {
                doc()->Parse<0>("{}");
                return false;
            }
            return true;
        }
        return false;
    }
#endif
};

inline KConfigValue::KConfigValue(RJsonValue *v, KConfig* c)
{
    assert(nullptr != c);
    assert(nullptr != v);

    jv_ = new RJsonValue(*v, c->GetAlloctor());
    cfg_ = c;
}

inline KConfigValue* KConfigValue::nodeValue(const QString& node_path)    // path/to/node
{
    RJsonValue v(*jval(), cfg_->GetAlloctor());
    for (const QString& p : node_path.split("/")) {
        QByteArray pm = p.toUtf8();
        if (v.HasMember(pm.constData())) {
            v = v.operator[](pm.constData());
        }
        else {
            return nullptr;
        }
    }
    KConfigValue* t = new KConfigValue(&v, cfg_);
    values_.push_back(t);
    return t;
}

inline KConfig* KConfigValue::NewConfig(const QString& json)
{
    KConfig *v = new KConfig(json);
    values_.push_back(v);
    return v;
}

inline KConfigValue& KConfigValue::_AddMember_internal(const String& name, RJsonValue& v)
{
    jv_->AddMember(_utf8_str(name), v, cfg_->GetAlloctor());
    return *this;
}

inline KConfigValue& KConfigValue::AddMember(const String& name, const char* val)
{
    assert(nullptr != val);
        
    RJsonValue _v(val, cfg_->GetAlloctor());
    return _AddMember_internal(name, _v);
}

inline KConfigValue& KConfigValue::AddMember(const String& name, const String& val)
{
    RJsonValue _v(val.toUtf8().constData(), cfg_->GetAlloctor());
    return _AddMember_internal(name, _v);
}

inline KConfigValue& KConfigValue::AddMember(const String& name, KConfigValue* val)
{
    assert(nullptr != val);
    assert(Exist(val));
    return _AddMember_internal(name, *(val->jval()));
}

KConfigValue& KConfigValue::PushBack(KConfigValue* v)
{
    jv_->PushBack(*(v->jval()), cfg_->GetAlloctor());

    return *this;
}

template <class _typ>
KConfigValue& KConfigValue::PushBack(_typ val)
{

	jv_->PushBack(val, cfg_->GetAlloctor());

    return *this;
}

inline KConfigValue& KConfigValue::PushBack(const String& str)
{
    jv_->PushBack(_utf8_str(str), cfg_->GetAlloctor());
    return *this;
}

#endif // config_h__
