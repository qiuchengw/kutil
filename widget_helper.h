#ifndef _KUTIL_WIDGET_HELPER_H_
#define _KUTIL_WIDGET_HELPER_H_

#include "macro.h"
#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>

class QWidget;
class QAbstractItemModel;
class QStandardItemModel;

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QHeaderView>
#include <QTableWidget>

namespace kutil
{
    namespace widget {

        // 窗口的桌面位置
        QRect windowRect(QWidget *w);

        QPoint GetWidgetScreenPos(QWidget* widget);

        /**
        *  为widget应用样式表
        *
        *	@return
        *		true    成功
        *	@param
        *		-[in]
        *      wdt   需要应用样式表的控件
        *      res   资源路径，可以是外部资源或者程序内建资源
        *
        **/
        bool ApplyStyleSheet(QWidget* wdt, const QString& res);

        // 获取资源
        QIcon GetResourceIcon(const QString& themeName, const QString& resouce_name);

        // 控件居中到pt位置
        void CenterWidget(QWidget* w, QPoint pt);

        // 控件居中到屏幕中央
        void CenterWidgetDesktop(QWidget* w);

        // 覆盖到屏幕上，不覆盖任务栏
        void MoveToFullWorkArea(QWidget* wdt);

        // 保存model内的数据为CSV，Execel表格
        void SaveToCSV(QStandardItemModel* m);

        //保存QTableWidget内的数据为CSV
        void SaveToCSV(QTableWidget*);

        //保存QList<QStringList>内的数据为CSV
        bool SaveToCSV(QList<QStringList>&);

        QList<QStringList> ReadFromCVS(QTextCodec *codec = nullptr);
        bool ReadFromCVS(QList<QStringList>& ctn, QTextCodec *codec = nullptr);

        // 复制model内的数据到剪贴板，
        void CopyTableModel2Clipboard(QStandardItemModel* m);

        // 复制QTableWidget到剪切板
        void CopyTableWidget2Clipboard(QTableWidget*);

        //判断float是否为0
        bool IsFloatZero(const float&);

        //判断double是否为0
        bool IsDoubleZero(const double&);

        //显示亏盈提示框 代码 市值 涨跌幅
        //void ShowProfitLoss(const QString&, const QString&,const QString&,
        //bool bNewPos = false);

        //显示平仓操作的界面
        void ShowClosePosView(const QString&, const QString&);

        // 清空表
        void ClearTable(QAbstractItemModel* m);

        // 判断控件w当前是否在具有焦点的窗口中（或者本身就是具有输入焦点的控件）
        bool IsInFocusingWindow(QWidget* w);

        // 获取ItemView当前可见的第一行，和最后一行的索引
        bool indexVisibleItem(QAbstractItemView* v, __p_out QModelIndex& idxFirst, __p_out QModelIndex& idxLast);
    }

    namespace widget {

        struct  TBLFiled
        {
            TBLFiled(const QString& title, const QString& data_filed, int width, int precision,
                QHeaderView::ResizeMode resize_m = QHeaderView::Interactive, QColor bk = QColor())
                :title_(title), data_filed_(data_filed), precision_(precision),
                resize_mode_(resize_m), width_(width), bk_(bk)
            {
            }

            QString     title_;
            // 对应的结构体名字
            QString     data_filed_;
            QHeaderView::ResizeMode resize_mode_ = QHeaderView::Interactive;
            int         width_ = 0;
            int         precision_ = -1;    // 列为double的时候设置精度
            QColor      bk_;    // 背景色
        };

        class KTableHelper
        {
        public:
            /*
            *	对所有选中的item(一行里面可能包含多个item)进行调用 f
            *      如果 f 返回false，则停止执行
            */
            template<typename TblType>
            static void ForeachSelectItem(TblType* v,
                std::function<bool(const QModelIndex& idx)> f)
            {
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedIndexes())
                {
                    if (!f(idx))
                    {
                        break;
                    }
                }
            }

            /*
            *	对所有选中的行进行调用 f
            *      如果 f 返回false，则停止执行
            */
            template<typename TblType>
            static void ForeachSelectRows(TblType* v,
                std::function<bool(const QModelIndex& idx)> f)
            {
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedRows())
                {
                    if (!f(idx))
                    {
                        break;
                    }
                }
            }

            /*
            *	获取与idx在同一行的 第 col 列的index
            */
            static QModelIndex ModelIndexOfCol(QAbstractItemModel *m,
                const QModelIndex& idx, int col)
            {
                if ((nullptr == m)
                    || !idx.isValid()
                    || (m->columnCount() <= col))
                {
                    Q_ASSERT(false);
                    return QModelIndex();
                }

                if (idx.column() == col)
                    return idx;

                return m->index(idx.row(), col, idx.parent());
            }

            template<typename TblType>
            static QVariant ItemData(TblType* v, const QModelIndex& idx,
                int col, int role = Qt::DisplayRole)
            {
                QAbstractItemModel* m = v->model();
                if (nullptr != m)
                {
                    QModelIndex i = ModelIndexOfCol(m, idx, col);
                    if (i.isValid())
                    {
                        return m->data(i, role);
                    }
                }
                return QVariant();
            }

            template < typename TblType >
            static void ClearTable(TblType * v)
            {
                QAbstractItemModel *m = v->model();
                if (nullptr != m)
                {
                    m->removeRows(0, m->rowCount());
                }
            }

            static void InitTableWidget(QTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size)
            {
                // 默认禁止手动编辑
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int i = 0;
                for (const TBLFiled& f : fields)
                {
                    v->horizontalHeader()->setSectionResizeMode(i, f.resize_mode_);
                    v->setHorizontalHeaderItem(i, new QTableWidgetItem(f.title_));
                    if (f.width_ != -1)
                    {
                        // v->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
                        v->setColumnWidth(i, f.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++i;
                }

                v->setMinimumSize(min_size);
                // v->resizeColumnsToContents();

                // 记录下来
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }

#ifdef QVIRTUALTABLEWIDGET_H
            static void InitTableWidget(QVirtualTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size)
            {
                Q_ASSERT(nullptr != v->model());

                // 默认禁止手动编辑
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int col = 0;
                for (const TBLFiled& tf : fields)
                {
                    v->setHorizontalHeaderItem(col, tf.title_);
                    // v->horizontalHeader()->setSectionResizeMode(col, tf.resize_mode_);
                    // v->setHorizontalHeaderItem(col, new QTableWidgetItem(tf.title_));
                    if (tf.bk_.isValid())
                    {
                        v->setColumnBkColor(col, tf.bk_);
                    }

                    if (tf.width_ != -1)
                    {
                        // v->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Interactive);
                        v->setColumnWidth(col, tf.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++col;
                }

                // 隐藏掉垂直的表头
                v->verticalHeader()->hide();

                // 选择的时候
                v->setSelectionMode(QAbstractItemView::SingleSelection);
                v->setSelectionBehavior(QAbstractItemView::SelectRows);

                // 刷新！！！，目前还不知道怎么回事，不刷新就不能显示表头
                v->setShowFixedRows(false);
                v->setShowFixedRows(true);
                //         v->setShowFilter(true);
                //         v->setShowFilter(false);
                v->setMinimumSize(min_size);

                col = 0;
                for (const TBLFiled& tf : fields)
                {
                    if (tf.width_ != -1)
                    {
                        v->setColumnWidth(col, tf.width_);
                    }

                    if (tf.precision_ != -1)
                    {
                        v->setColumnDoublePrecision(col, tf.precision_);
                    }
                    ++col;
                }
                v->setShowFilter(true);
                v->setShowFilter(false);
                // v->resizeColumnsToContents();

                // 记录下来
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }
#endif

            static QList<TBLFiled> Fileds(const QWidget* v)
            {
                return table_fields_.value(v);
            }

#define CODE_ID_FILED QStringLiteral("#code")
#define INVALID_INDEX (-1)

            template<typename _Tbl>
            static int FindCol(_Tbl* tbl, const QString& filed)
            {
                int col = 0;
                for (const TBLFiled& t : KTableHelper::Fileds(tbl))
                {
                    if (filed == t.data_filed_)
                    {
                        return col; // 找到了
                    }
                    col++;
                }
                return INVALID_INDEX;
            }

            static int UniqueCol(const QWidget* v)
            {
                return FindCol(v, table_unique_col_.value(v));
            }

            template<typename _Tbl>
            static int FindRow(_Tbl* t, const QString& s_code)
            {
                int code_filed = KTableHelper::UniqueCol(t);
                if (INVALID_INDEX == code_filed)
                {
                    return INVALID_INDEX;
                }

                int rows = t->rowCount();
                for (int i = 0; i < rows; ++i)
                {
                    if (t->getCellText(i, code_filed) == s_code)
                    {
                        return i;
                    }
                }
                return -1;
            }

        private:
            static QHash<const QWidget*, QList<TBLFiled>> table_fields_;
            // 唯一列索引
            static QHash<const QWidget*, QString> table_unique_col_;
        };
    }

};

#endif // _KUTIL_WIDGET_HELPER_H_
