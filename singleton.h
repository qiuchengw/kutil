#ifndef singleton_h__
#define singleton_h__

#include <mutex>

// �ο���http://ju.outofmemory.cn/entry/131036
// ��ʵ��ģʽ
template<class T>
class Singleton
{
public:
    static T* instance()
    {
        std::call_once(flag_, [](T** inst) {
            *inst = new T;
            _desctructorDelegate();
        }, &instance_);
        return instance_;
    }

private:
    // �����Զ�delete��ʵ������
    static void _desctructorDelegate()
    {
        struct _Destructor
        {
        public:
            _Destructor(T* t)
                :t_(t)
            {
            }

            ~_Destructor()
            {
                delete t_;
            }
        private:
            T* t_;
        };
        static _Destructor _auto_del(instance_);
    }

protected:
    Singleton(){}
    virtual ~Singleton()
    {
    }

    static T* instance_;
    static std::once_flag flag_;

    /**
    * ��ֹ��������
    */
private:
    Singleton(const Singleton &) = delete;
    Singleton & operator = (const Singleton &) = delete;
};
template<typename T>
T* Singleton<T>::instance_ = nullptr;

template<typename T>
std::once_flag Singleton<T>::flag_;

//////////////////////////////////////////////////////////////////////////
template<typename T, typename BaseClass>
class SingletonWithBase : public BaseClass
{
public:
    static T* instance()
    {
        std::call_once(flag_, [](T** inst) {
            *inst = new T;
            _desctructorDelegate();
        }, &instance_);
        return instance_;
    }

private:
    // �����Զ�delete��ʵ������
    static void _desctructorDelegate()
    {
        struct _Destructor
        {
        public:
            _Destructor(T* t)
                :t_(t)
            {
            }

            ~_Destructor()
            {
                delete t_;
            }
        private:
            T* t_;
        };
        static _Destructor _auto_del(instance_);
    }

    // ���� vs2013 ����֧�֣���
    // �Բۣ�

//     static T* Singleton::instance()
//     {
//         static T _i;
//         return &_i;
//     }

protected:
    template<class... Args>
    SingletonWithBase(Args&&... args)
        :BaseClass(std::forward<Args>(args)...){}
    virtual ~SingletonWithBase()
    {
    }

    static T* instance_;
    static std::once_flag flag_;

    /**
    * ��ֹ��������
    */
private:
    SingletonWithBase(const SingletonWithBase &) = delete;
    SingletonWithBase & operator = (const SingletonWithBase &) = delete;
};
template<typename T, typename B>
T* SingletonWithBase<T, B>::instance_ = nullptr;

template<typename T, typename B>
std::once_flag SingletonWithBase<T, B>::flag_;

// template<typename T>
// typedef SingletonWithBase<T, QObject> SingletonObject;

#endif // singleton_h__
