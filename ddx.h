#ifndef ddx_h__
#define ddx_h__

#include <functional>
#include <QList>
#include <QComboBox>
#include <QTextEdit>
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractButton>

#ifndef QDATETIME_H
#include <QDateTime>
#include <QTime>
#include <QDate>
#endif

#ifndef QDATETIMEEDIT_H
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#endif

struct ddx_base
{
    /**
     *  pull    : widget -> value		
     *  push    : value  -> widget
     *
    **/
    virtual void operator()(bool pull) = 0;
};

struct ddx_function : public ddx_base
{
    ddx_function(QWidget* w, std::function<void(QWidget*, bool)> f)
        :w_(w), f_(f)
    {
        Q_ASSERT(nullptr != w);
    }

    virtual void operator()(bool pull)
    {
        f_(w_, pull);
    }

    QWidget* w_ = nullptr;
    std::function<void(QWidget*, bool)> f_;
};

template<class _W, class _V>
struct ddx_ : public ddx_base
{
    ddx_(_W* w, _V& v)
        :w_(w), v_(v)
    {
        Q_ASSERT(nullptr != w);
    }

    _W* w_ = nullptr;
    _V& v_;
};

template<typename _V>
struct _string_functor
{
    QString operator()(const _V& o) { return QString::number(o); }
};

struct _null_functor
{
    QString operator()(const QString& o) { return o;  }
};

struct _int_functor
{
    int operator()(const QString& o) { return o.toInt(0); }
};

struct _float_functor
{
    float operator()(const QString& o) { return o.toFloat(0); }
};

struct _double_functor
{
    double operator()(const QString& o) { return o.toDouble(0); }
};

// int
template<class _W, typename _V, class _F = _null_functor >
struct ddx_text_template : public ddx_<_W, _V>
{
    typedef ddx_<_W, _V> _Base;

    ddx_text_template(_W* w, _V & v) :ddx_<_W, _V>(w, v) { }

    virtual void operator()(bool pull){
        if (pull) 
            _Base::v_ = _F()(_Base::w_->text());
        else 
            _Base::w_->setText(QString::number(_Base::v_));
    }
};

typedef ddx_text_template<QLineEdit, int, _int_functor> ddx_lineedit_int;
typedef ddx_text_template<QLineEdit, float, _float_functor> ddx_lineedit_float;
typedef ddx_text_template<QLineEdit, double, _double_functor> ddx_lineedit_double;

// text
template<class _W>
struct ddx_text : public ddx_ < _W, QString >
{
    typedef ddx_<_W, QString> _Base;

    ddx_text(_W* w, QString & v) :ddx_<_W, QString>(w, v) { }

    virtual void operator()(bool pull){
        if (pull)
            _Base::v_ = _Base::w_->text();
        else 
            _Base::w_->setText(_Base::v_);
    }
};
typedef ddx_text<QLineEdit> ddx_lineedit_text;
typedef ddx_text<QLabel> ddx_label_text;

template<class _W>
struct ddx_edit_text_ : public ddx_ < _W, QString >
{
    typedef ddx_<_W, QString> _Base;

    ddx_edit_text_(_W* w, QString & v) :ddx_<_W, QString>(w, v) { }

    virtual void operator()(bool pull)
    {
        if (pull) 
            _Base::v_ = _Base::w_->toPlainText();
        else 
            _Base::w_->setText(_Base::v_);
    }
};
typedef ddx_edit_text_<QTextEdit> ddx_textedit_text;
typedef ddx_edit_text_<QTextBrowser> ddx_textbrowser_text;
typedef ddx_edit_text_<QPlainTextEdit> ddx_paintext_text;

//------------------------
// date time
struct ddx_date : public ddx_ < QDateEdit, QDate >
{
    typedef ddx_<QDateEdit, QDate> _Base;

    ddx_date(QDateEdit* w, QDate & v) :ddx_<QDateEdit, QDate >(w, v) { }

    virtual void operator()(bool pull){
        if (pull) 
            _Base::v_ = _Base::w_->date();
        else 
            _Base::w_->setDate(_Base::v_);
    }
};

struct ddx_time : public ddx_ < QTimeEdit, QTime >
{
    typedef ddx_<QTimeEdit, QTime> _Base;

    ddx_time(QTimeEdit* w, QTime & v) :ddx_<QTimeEdit, QTime >(w, v) { }

    virtual void operator()(bool pull){
        if (pull) _Base::v_ = _Base::w_->time();
        else _Base::w_->setTime(_Base::v_);
    }
};

struct ddx_datetime : public ddx_ < QDateTimeEdit, QDateTime >
{
    typedef ddx_<QDateTimeEdit, QDateTime> _Base;

    ddx_datetime(QDateTimeEdit* w, QDateTime & v) :ddx_<QDateTimeEdit, QDateTime >(w, v) { }

    virtual void operator()(bool pull){
        if (pull) _Base::v_ = _Base::w_->dateTime();
        else _Base::w_->setDateTime(_Base::v_);
    }
};

//////////////////////////////////////////////////////////////////////////
// Combobox
template<typename _T>
struct ddx_cmb_index : public ddx_ < QComboBox, _T >
{
    typedef ddx_<QComboBox, _T> _Base;

    ddx_cmb_index(QComboBox* w, _T & v) :ddx_<QComboBox, _T>(w, v) { }

    virtual void operator()(bool pull){
        if (pull) _Base::v_ = _Base::w_->currentIndex();
        else _Base::w_->setCurrentIndex(_Base::v_);
    }
};

struct ddx_cmb_text : public ddx_ < QComboBox, QString >
{
    typedef ddx_<QComboBox, QString> _Base;

    ddx_cmb_text(QComboBox* w, QString & v) :ddx_<QComboBox, QString>(w, v) { }

    virtual void operator()(bool pull){
        if (pull) _Base::v_ = _Base::w_->currentText();
        else _Base::w_->setCurrentText(_Base::v_);
    }
};

//////////////////////////////////////////////////////////////////////////
// QDoubleSpinBox
struct ddx_doublespinbox : public ddx_ < QDoubleSpinBox, double >
{
		ddx_doublespinbox(QDoubleSpinBox* w, double & v) :ddx_<QDoubleSpinBox, double >(w, v) { }

		virtual void operator()(bool pull){
				if (pull) v_ = w_->value();
				else w_->setValue(v_);
		}
};

//QSpinBox
struct ddx_spinbox : public ddx_ < QSpinBox, int >
{
		ddx_spinbox(QSpinBox* w, int & v) :ddx_<QSpinBox, int >(w, v) { }

		virtual void operator()(bool pull){
				if (pull) v_ = w_->value();
				else w_->setValue(v_);
		}
};

//////////////////////////////////////////////////////////////////////////
// checker
template<typename _W, typename _T = bool>
struct ddx_checker : public ddx_ < _W, _T >
{
    typedef ddx_<_W, _T> _Base;

    ddx_checker(_W* w, _T & v) :ddx_<_W, _T>(w, v) { }

    virtual void operator()(bool pull){
        if (pull) _Base::v_ = _Base::w_->isChecked();
        else _Base::w_->setChecked(_Base::v_);
    }
};
typedef ddx_checker<QAbstractButton> ddx_checker_button;
typedef ddx_checker<QGroupBox> ddx_checker_group;

//////////////////////////////////////////////////////////////////////////
struct _ddxmap
{
    typedef QList<ddx_base*> dgroup;
    typedef QHash<int, dgroup*> dmap;

    ~_ddxmap()
    {
        for (auto i = m_.begin(); i != m_.end(); ++i)
        {
            for (ddx_base* b : *(i.value()))
            {
                delete b;
            }
            delete i.value();
        }
    }

    inline dmap* map()
    {
        return &m_;
    }

    inline dgroup* group(int gid)
    {
        return map()->value(gid, nullptr);
    }

    inline void add(int gid, ddx_base* b)
    {
        Q_ASSERT(nullptr != b);
        dgroup *p = group(gid);
        if (nullptr == p)
        {
            p = new dgroup;
            map()->insert(gid, p);
        }
        p->push_back(b);
    }

private:
    dmap m_;
};

// 头文件内定义此宏，可以得到DDX支持
#define Q_DDX_SUPPORT \
    _ddxmap __m_ddx_map__; \
    _ddxmap* _ddxmap_() { return &__m_ddx_map__; }\
    void DDX_Function(QWidget* w, std::function<void(QWidget*, bool)> f, int g_id=0) {\
        _ddxmap_()->add(g_id, new ddx_function(w, f)); \
    }\
    void DDX_PULL(int g_id = 0) { \
        auto *p = _ddxmap_()->group(g_id);\
        if (nullptr == p) return;\
        for (ddx_base *b : *p){ (*b)(true) ;}\
    };\
    void DDX_PUSH(int g_id = 0) { \
        auto *p = _ddxmap_()->group(g_id);\
        if (nullptr == p) return;\
        for (ddx_base *b : *p){ (*b)(false) ;}\
    };

//////////////////////////////////////////////////////////////////////////
// 在适当的时候构造DDX宏
// edit number
#define DDX_LineEdit_Int_g(w, v, g) _ddxmap_()->add(g, new ddx_lineedit_int(w, v))
#define DDX_LineEdit_Int(w, v) DDX_LineEdit_Int_g(w,v, 0)

#define DDX_LineEdit_Float_g(w, v, g) _ddxmap_()->add(g, new ddx_lineedit_float(w, v))
#define DDX_LineEdit_Float(w, v) DDX_LineEdit_Float_g(w,v, 0)

#define DDX_LineEdit_Double_g(w, v, g) _ddxmap_()->add(g, new ddx_lineedit_double(w, v))
#define DDX_LineEdit_Double(w, v) DDX_LineEdit_Double_g(w,v, 0)

//////////////////////////////////////////////////////////////////////////
// text
#define DDX_LineEdit_Text_g(w, v, g) _ddxmap_()->add(g, new ddx_lineedit_text(w, v))
#define DDX_LineEdit_Text(w, v) DDX_LineEdit_Text_g(w, v, 0)

#define DDX_TextEdit_Text_g(w, v, g) _ddxmap_()->add(g, new ddx_textedit_text(w, v))
#define DDX_TextEdit_Text(w, v) DDX_TextEdit_Text_g(w, v, 0)

#define DDX_Label_Text_g(w, v, g) _ddxmap_()->add(g, new ddx_label_text(w, v))
#define DDX_Label_Text(w, v) DDX_Label_Text_g(w, v, 0)

#define DDX_PlainText_Text_g(w, v, g) _ddxmap_()->add(g, new ddx_paintext_text(w, v))
#define DDX_PlainText_Text(w, v) DDX_PlainText_Text_g(w, v, 0)

//////////////////////////////////////////////////////////////////////////
// date time
#define DDX_DateEdit_g(w, v, g) _ddxmap_()->add(g, new ddx_date(w, v))
#define DDX_DateEdit(w, v) DDX_DateEdit_g(w, v, 0)

#define DDX_TimeEdit_g(w, v, g) _ddxmap_()->add(g, new ddx_time(w, v))
#define DDX_TimeEdit(w, v) DDX_TimeEdit_g(w, v, 0)

#define DDX_DateTimeEdit_g(w, v, g) _ddxmap_()->add(g, new ddx_datetime(w, v))
#define DDX_DateTimeEdit(w, v) DDX_DateTimeEdit_g(w, v, 0)

//////////////////////////////////////////////////////////////////////////
// combobox
#define DDX_Cmb_Index_g(w, v, g) _ddxmap_()->add(g, new ddx_cmb_index<decltype(v)>(w, v))
#define DDX_Cmb_Index(w, v) DDX_Cmb_Index_g(w, v, 0)

#define DDX_Cmb_Text_g(w, v, g) _ddxmap_()->add(g, new ddx_cmb_text(w, v))
#define DDX_Cmb_Text(w, v) DDX_Cmb_Text_g(w, v, 0)

//////////////////////////////////////////////////////////////////////////
// checker
#define DDX_Button_Checker_g(w, v, g) _ddxmap_()->add(g, new ddx_checker_button(w, v)) 
#define DDX_Button_Checker(w, v) DDX_Button_Checker_g(w, v, 0)

#define DDX_Group_Checker_g(w, v, g) _ddxmap_()->add(g, new ddx_checker_group(w, v))
#define DDX_Group_Checker(w, v) DDX_Group_Checker_g(w, v, 0)

//////////////////////////////////////////////////////////////////////////
// QDoubleSpinBox
#define DDX_DoubleSpinBox_g(w, v, g) _ddxmap_()->add(g, new ddx_doublespinbox(w, v))
#define DDX_DoubleSpinBox(w, v) DDX_DoubleSpinBox_g(w, v, 0)

#define DDX_IntSpinBox_g(w, v, g) _ddxmap_()->add(g, new ddx_spinbox(w, v))
#define DDX_SpinBox(w, v) DDX_IntSpinBox_g(w, v, 0)

//////////////////////////////////////////////////////////////////////////
// functional

#define DDX_Function_g(w, v, g) _ddxmap_()->add(g, new ddx_cmb_text(w, v))
#define DDX_Cmb_Text(w, v) DDX_Cmb_Text_g(w, v, 0)

#endif // ddx_h__
