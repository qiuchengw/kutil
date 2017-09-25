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
#include <QApplication>
#include <QFileDialog>

class QWidget;
class QAbstractItemModel;
class QStandardItemModel;

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QHeaderView>
#include <QTableWidget>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include <QClipboard>

namespace kutil
{
    namespace widget {

        // ���ڵ�����λ��
        inline QRect windowRect(QWidget *w) {
            QRect r = w->rect();
            r.moveTo(w->mapToGlobal(r.topLeft()));
            return r;
        }

        inline QPoint getWidgetScreenPos(QWidget* widget) {
            return windowRect(widget).topLeft();
        }

        /**
        *  ΪwidgetӦ����ʽ��
        *
        *	@return
        *		true    �ɹ�
        *	@param
        *		-[in]
        *      wdt   ��ҪӦ����ʽ��Ŀؼ�
        *      res   ��Դ·�����������ⲿ��Դ���߳����ڽ���Դ
        *
        **/
        inline bool applyStyleSheet(QWidget* wdt, const QString& res) {
            Q_ASSERT(wdt != nullptr);
            Q_ASSERT(!res.isEmpty());

            if (!QFile::exists(res))
                return false;

            QFile file(res);
            if (file.open(QFile::ReadOnly)){
                if (file.isOpen()){
                    wdt->setStyleSheet(QTextStream(&file).readAll());
                    return true;
                }
            }
            return false;
        }

        // ��ȡ��Դ
        inline QIcon getResourceIcon(const QString& themeName, const QString& resouce_name) {
            return QIcon(themeName + resouce_name);
        }

        // �ؼ����е�ptλ��
        inline void centerWidget(QWidget* w, QPoint pt) {
            if (nullptr == w){
                Q_ASSERT(false);
                return;
            }

            QRect rc = w->rect();
            rc.moveCenter(pt);
            w->move(rc.topLeft());
        }

        // �ؼ����е���Ļ����
        inline void centerWidgetDesktop(QWidget* w) {
            centerWidget(w, QApplication::desktop()->availableGeometry().center());
        }

        // ���ǵ���Ļ�ϣ�������������
        inline void moveToFullWorkArea(QWidget* wdt) {
            if (nullptr != wdt){
                wdt->setGeometry(QApplication::desktop()->availableGeometry());
            }
        }

        // ����model�ڵ�����ΪCSV��Execel���
        inline void saveToCSV(QStandardItemModel* m) {
            if (m->rowCount() != 0){
                QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("���ļ�"),
                    "", "*.csv", 0, QFileDialog::DontUseNativeDialog);
                QString file = filepath + ".csv";

                //���дcsv���ݱ������
                QList<QString> str_list_;
                //��ȡ����
                QString str_head_;
                for (int i = 0; i < m->columnCount(); i++){
                    QStandardItem* iitem = m->horizontalHeaderItem(i);
                    if (i == m->columnCount() - 1){
                        str_head_.append(iitem->text());
                        str_head_.append("\n");
                    }
                    else{
                        str_head_.append(iitem->text());
                        str_head_.append(",");
                    }
                }

                str_list_.push_back(str_head_);
                //��ȡ����
                for (int i = 0; i < m->rowCount(); i++){
                    QString str;
                    for (int j = 0; j < m->columnCount(); j++){
                        QStandardItem* iitem = m->item(i, j);
                        if (j == m->columnCount() - 1){
                            str.append(iitem->text());
                            str.append("\n");
                        }
                        else{
                            str.append(iitem->text());
                            str.append(",");
                        }

                    }
                    // // qDebug() << str_;
                    str_list_.push_back(str);
                }

                QFile csvFile(file);
                if (csvFile.open(QIODevice::ReadWrite)){
                    QTextStream out(&csvFile);
                    //���ֻ��д�λ������Ϊ�ַ����к��� \r,\n�ַ�
                    for (int i = 0; i < str_list_.count(); i++){
                        QString str_temp = str_list_.at(i);
                        if (str_temp.contains("\r") || str_temp.contains("\n")){
                            str_temp.remove("\r", Qt::CaseInsensitive).remove("\n", Qt::CaseInsensitive);
                        }
                        out << str_temp << "\n";
                    }
                }
            }
        }

        //����QTableWidget�ڵ�����ΪCSV
        inline void saveToCSV(QTableWidget*tbl) {
            if (0 == tbl->rowCount())
                return;

            QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("���ļ�"),
                "", "*.csv", 0, QFileDialog::DontUseNativeDialog);
            if (filepath.isEmpty()){
                return;
            }
            QString file = filepath + ".csv";

            //���дcsv���ݱ������
            QList<QString> str_list_;
            for (int i = -1; i < tbl->rowCount(); i++){
                QString str;
                for (int j = 0; j < tbl->columnCount(); j++){
                    if (-1 == i){
                        //����
                        str.append(tbl->horizontalHeaderItem(j)->text());
                    }
                    else{
                        //����
                        str.append(tbl->item(i, j)->text());
                    }

                    j == (tbl->columnCount() - 1) ?
                        str.append("\n") : str.append(",");
                }
                str_list_.push_back(str);
            }

            QFile csvFile(file);
            if (csvFile.open(QIODevice::ReadWrite)){
                QTextStream out(&csvFile);
                //���ֻ��д�λ������Ϊ�ַ����к��� \r,\n�ַ�
                for (int i = 0; i < str_list_.count(); i++){
                    QString str_temp = str_list_.at(i);
                    str_temp.replace("\r\n", "");
                    str_temp.replace("\r", "");
                    str_temp.replace("\n", "");
                    out << str_temp << "\n";
                }
            }
        }

        //����QList<QStringList>�ڵ�����ΪCSV
        inline bool saveToCSV(QList<QStringList>&content) {
            QString filepath = QFileDialog::getSaveFileName(nullptr, QStringLiteral("���ļ�"),
                "", "*.csv", 0, QFileDialog::DontUseNativeDialog);
            if (filepath.isEmpty()){
                return false;
            }
            QString file = filepath + ".csv";
            QFile csvFile(file);
            if (csvFile.open(QIODevice::ReadWrite)){
                QTextStream out(&csvFile);
                //���ֻ��д�λ������Ϊ�ַ����к��� \r,\n�ַ�
                for (int i = 0; i < content.count(); i++){
                    QString str_temp = content.at(i).join(",");
                    str_temp.replace("\r\n", "");
                    str_temp.replace("\r", "");
                    str_temp.replace("\n", "");
                    out << str_temp << "\n";
                }
            }
            return true;
        }

        // ����model�ڵ����ݵ������壬
        inline void copyTableModel2Clipboard(QStandardItemModel* m) {
            int row = m->rowCount();
            int col = m->columnCount();
            if (row <= 0) {
                return;
            }

            QString str_copy;
            //����ͷ����
            for (int k = 0; k < col; k++) {
                str_copy.append(m->horizontalHeaderItem(k)->text());
                if (k == col - 1) {
                    str_copy.append("\n");
                }
                else {
                    str_copy.append(",");
                }
            }

            //��������
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    str_copy.append(m->item(i, j)->text());
                    if (j == col - 1) {
                        str_copy.append("\n");
                    }
                    else {
                        str_copy.append(",");
                    }
                }
            }
            QClipboard *board = QApplication::clipboard();
            board->setText(str_copy);
        }

        // ����QTableWidget�����а�
        inline void copyTableWidget2Clipboard(QTableWidget* tbl) {
            if (0 != tbl->rowCount()) {
                if (-1 != tbl->currentIndex().row())
                {
                    QString str;
                    for (int i = 0; i < tbl->columnCount(); i++)
                    {
                        str.append(tbl->item(tbl->currentIndex().row(), i)->text());
                        str.append("  ");
                    }

                    QClipboard *board = QApplication::clipboard();
                    board->setText(str);
                }
            }
        }

        // ��ձ�
        inline void clearTable(QAbstractItemModel* m) {
            for (int i = m->rowCount(); i >= 0; i--){
                m->removeRow(i);
            }
        }

        // �жϿؼ�w��ǰ�Ƿ��ھ��н���Ĵ����У����߱�����Ǿ������뽹��Ŀؼ���
        inline bool isInFocusingWindow(QWidget* w) {
            if (nullptr != w){
                if (QWidget* focus_w = QApplication::focusWidget()){
                    if ((w == focus_w) || w->isAncestorOf(focus_w))
                        return true;

                    return focus_w->window()->isAncestorOf(w);
                }
            }
            return false;
        }

        // ��ȡItemView��ǰ�ɼ��ĵ�һ�У������һ�е�����
        inline bool indexVisibleItem(QAbstractItemView* v, __p_out QModelIndex& idxFirst, __p_out QModelIndex& idxLast) {
            if (nullptr == v){
                Q_ASSERT(false);
                return false;
            }

            if (QAbstractItemModel* m = v->model()){
                QRect rc = v->rect();
                idxFirst = v->indexAt(rc.topLeft());
                if (!idxFirst.isValid())
                    return false;

                idxLast = v->indexAt(rc.bottomLeft());
                if (!idxLast.isValid()){
                    idxLast = idxFirst.sibling(m->rowCount() - 1, idxFirst.column());
                }
                return true;
            }
            return true;
        }

		inline bool bring2TopWidget(QWidget* w){
			if (nullptr == w){
				Q_ASSERT(false);
				return false;
			}

			// ��������ʾ����
			QWidget* win = w->window();
			// �п�����С���ˣ��Ȼָ���ͨ״̬
			if (win->windowState() & Qt::WindowMinimized){
				win->setWindowState((w->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
			}

			// ����ʾ
			win->show();
			// �����
			w->activateWindow();
			// ȷ�������ˡ�
			win->raise();

			return true;
		}

    }

    namespace widget {

        struct  TBLFiled
        {
            TBLFiled(const QString& title, const QString& data_filed, int width, int precision,
                QHeaderView::ResizeMode resize_m = QHeaderView::Interactive, QColor bk = QColor())
                :title_(title), data_filed_(data_filed), precision_(precision),
                resize_mode_(resize_m), width_(width), bk_(bk){
            }

            QString     title_;
            // ��Ӧ�Ľṹ������
            QString     data_filed_;
            QHeaderView::ResizeMode resize_mode_ = QHeaderView::Interactive;
            int         width_ = 0;
            int         precision_ = -1;    // ��Ϊdouble��ʱ�����þ���
            QColor      bk_;    // ����ɫ
        };

        class KTableHelper
        {
        public:
            /*
            *	������ѡ�е�item(һ��������ܰ������item)���е��� f
            *      ��� f ����false����ִֹͣ��
            */
            template<typename TblType>
            static void ForeachSelectItem(TblType* v, std::function<bool(const QModelIndex& idx)> f){
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedIndexes()){
                    if (!f(idx)){
                        break;
                    }
                }
            }

            /*
            *	������ѡ�е��н��е��� f
            *      ��� f ����false����ִֹͣ��
            */
            template<typename TblType>
            static void ForeachSelectRows(TblType* v, std::function<bool(const QModelIndex& idx)> f){
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedRows()){
                    if (!f(idx)){
                        break;
                    }
                }
            }

            /*
            *	��ȡ��idx��ͬһ�е� �� col �е�index
            */
            static QModelIndex ModelIndexOfCol(QAbstractItemModel *m, const QModelIndex& idx, int col){
                if ((nullptr == m)
                    || !idx.isValid()
                    || (m->columnCount() <= col)){
                    Q_ASSERT(false);
                    return QModelIndex();
                }

                if (idx.column() == col)
                    return idx;

                return m->index(idx.row(), col, idx.parent());
            }

            template<typename TblType>
            static QVariant ItemData(TblType* v, const QModelIndex& idx,
                int col, int role = Qt::DisplayRole){
                QAbstractItemModel* m = v->model();
                if (nullptr != m){
                    QModelIndex i = ModelIndexOfCol(m, idx, col);
                    if (i.isValid()){
                        return m->data(i, role);
                    }
                }
                return QVariant();
            }

            template < typename TblType >
            static void ClearTable(TblType * v){
                QAbstractItemModel *m = v->model();
                if (nullptr != m){
                    m->removeRows(0, m->rowCount());
                }
            }

            static void InitTableWidget(QTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size){
                // Ĭ�Ͻ�ֹ�ֶ��༭
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int i = 0;
                for (const TBLFiled& f : fields){
                    v->horizontalHeader()->setSectionResizeMode(i, f.resize_mode_);
                    v->setHorizontalHeaderItem(i, new QTableWidgetItem(f.title_));
                    if (f.width_ != -1){
                        // v->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
                        v->setColumnWidth(i, f.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++i;
                }

                v->setMinimumSize(min_size);
                // v->resizeColumnsToContents();

                // ��¼����
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }

#ifdef QVIRTUALTABLEWIDGET_H
            static void InitTableWidget(QVirtualTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size){
                Q_ASSERT(nullptr != v->model());

                // Ĭ�Ͻ�ֹ�ֶ��༭
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int col = 0;
                for (const TBLFiled& tf : fields){
                    v->setHorizontalHeaderItem(col, tf.title_);
                    // v->horizontalHeader()->setSectionResizeMode(col, tf.resize_mode_);
                    // v->setHorizontalHeaderItem(col, new QTableWidgetItem(tf.title_));
                    if (tf.bk_.isValid()){
                        v->setColumnBkColor(col, tf.bk_);
                    }

                    if (tf.width_ != -1){
                        // v->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Interactive);
                        v->setColumnWidth(col, tf.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++col;
                }

                // ���ص���ֱ�ı�ͷ
                v->verticalHeader()->hide();

                // ѡ���ʱ��
                v->setSelectionMode(QAbstractItemView::SingleSelection);
                v->setSelectionBehavior(QAbstractItemView::SelectRows);

                // ˢ�£�������Ŀǰ����֪����ô���£���ˢ�¾Ͳ�����ʾ��ͷ
                v->setShowFixedRows(false);
                v->setShowFixedRows(true);
                //         v->setShowFilter(true);
                //         v->setShowFilter(false);
                v->setMinimumSize(min_size);

                col = 0;
                for (const TBLFiled& tf : fields){
                    if (tf.width_ != -1){
                        v->setColumnWidth(col, tf.width_);
                    }

                    if (tf.precision_ != -1){
                        v->setColumnDoublePrecision(col, tf.precision_);
                    }
                    ++col;
                }
                v->setShowFilter(true);
                v->setShowFilter(false);
                // v->resizeColumnsToContents();

                // ��¼����
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }
#endif

            static QList<TBLFiled> Fileds(const QWidget* v){
                return table_fields_.value(v);
            }

#define CODE_ID_FILED QStringLiteral("#code")
#define INVALID_INDEX (-1)

            template<typename _Tbl>
            static int FindCol(_Tbl* tbl, const QString& filed){
                int col = 0;
                for (const TBLFiled& t : KTableHelper::Fileds(tbl)){
                    if (filed == t.data_filed_){
                        return col; // �ҵ���
                    }
                    col++;
                }
                return INVALID_INDEX;
            }

            static int UniqueCol(const QWidget* v){
                return FindCol(v, table_unique_col_.value(v));
            }

            template<typename _Tbl>
            static int FindRow(_Tbl* t, const QString& s_code){
                int code_filed = KTableHelper::UniqueCol(t);
                if (INVALID_INDEX == code_filed){
                    return INVALID_INDEX;
                }

                int rows = t->rowCount();
                for (int i = 0; i < rows; ++i){
                    if (t->getCellText(i, code_filed) == s_code){
                        return i;
                    }
                }
                return -1;
            }

        private:
            static QHash<const QWidget*, QList<TBLFiled>> table_fields_;
            // Ψһ������
            static QHash<const QWidget*, QString> table_unique_col_;
        };
        __declspec(selectany) QHash<const QWidget*, QList<TBLFiled>> KTableHelper::table_fields_;
        __declspec(selectany) QHash<const QWidget*, QString> KTableHelper::table_unique_col_;
    }

};

#endif // _KUTIL_WIDGET_HELPER_H_
