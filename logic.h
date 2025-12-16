#ifndef LOGIC_H
#define LOGIC_H
#include <vector>
#include <QObject>
#include <QTableWidget>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <stdexcept>
#include <QTextStream>
#include <map>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#define DAES256 1
#define CBC 1
#define CTR 1
#define ECB 1
#include "aes.h"

using std::vector;
struct ManagerRow
{
    QString Name;
    QString Login;
    QString Pass;
    QString Comment;
};

QString& getVariableByColumn(int column, ManagerRow& row);

class Manager : public QObject
{
    Q_OBJECT
private:
    vector<ManagerRow> m_allRows;
    QTableWidget* m_table;
    bool m_changed = false;
    QObject* m_parent;
protected:
    void bindData()
    {
        // Отключить старый сигнал
        disconnect(m_table, &QTableWidget::cellClicked, nullptr, nullptr);
        disconnect(m_table, &QTableWidget::currentItemChanged, nullptr, nullptr);

        // Выбирать только одно
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);

        // По-новой подключить
        connect(m_table, &QTableWidget::cellClicked, [&] (int row, int column) { activated(row, column); });
        connect(m_table, &QTableWidget::currentItemChanged, [&] (QTableWidgetItem *current, QTableWidgetItem *previous) { changed(current, previous); });
    }
    void placeEdit(int row, int column, const QString& text)
    {
        QLineEdit *edit = new QLineEdit(m_table);
        edit->setText(text);
        edit->selectAll();
        edit->setFocus(Qt::OtherFocusReason);
        connect(edit, &QLineEdit::textEdited,
                [&, row, column] (const QString &text)
                {
            qDebug() << "hit";
            QString& str = getVariableByColumn(column, m_allRows[row]);
            if (str != text)
            {
                m_changed = true;
                str = text;
            }
        });
        connect(edit, &QLineEdit::editingFinished,
                [&] () {
                    qDebug() << "Finished";
                    fillTable();
                }
        );
        m_table->setCellWidget(row, column, edit);
    }
    void activated(int row, int column)
    {
        qDebug() << row << " " << column << " activated";
        fillTableInner();
        if (row >= m_allRows.size())
            return;
        QString& str = getVariableByColumn(column, m_allRows[row]);
        placeEdit(row, column, str);
    }
    void changed(QTableWidgetItem *current, QTableWidgetItem *previous)
    {
        if (previous == nullptr or current == nullptr)
            return;
        if (current->text() == previous ->text())
            return;
        if (current->row() == previous->row() and current->column() == previous->column())
            qDebug() << current->text() << " " << previous ->text() << " " << current->row() << " " << current->column() << " changed";
    }
    void copy(int row, int column)
    {
        QString& str = getVariableByColumn(column, m_allRows[row]);

        qDebug() << str << " copy";
    }
    void fillTableValues()
    {
        for (size_t i = 0; i < m_allRows.size(); i++)
        {
            m_table->setItem(i, 0, new QTableWidgetItem(m_allRows[i].Name));
            m_table->setItem(i, 1, new QTableWidgetItem("###"));

            QPushButton* buttonLoginCopy = new QPushButton("Copy");
            connect(buttonLoginCopy, &QPushButton::clicked, [&, i] () { qDebug() << "yo"; copy(i, 1); });
            m_table->setCellWidget(i, 2, buttonLoginCopy);

            m_table->setItem(i, 3, new QTableWidgetItem("###"));

            QPushButton* buttonPassCopy = new QPushButton("Copy");
            connect(buttonPassCopy, &QPushButton::clicked, [&, i] () { qDebug() << "yo"; copy(i, 3); });
            m_table->setCellWidget(i, 4, buttonPassCopy);

            m_table->setItem(i, 5, new QTableWidgetItem("###"));

            QPushButton* buttonRemove = new QPushButton("Remove");
            connect(buttonRemove, &QPushButton::clicked, [&, i] () { removeItem(i); });
            m_table->setCellWidget(i, 6, buttonRemove);
        }
        QPushButton* buttonAdd = new QPushButton("Add");
        // У нас +1 строка дял этой кнопки
        int lastRow = m_allRows.size();
        connect(buttonAdd, &QPushButton::clicked, [&] () { addItem(); });
        m_table->setCellWidget(lastRow, 3, buttonAdd);


    }
    void fillTableInner()
    {
        m_table->clear();
        m_table->setColumnCount(7);
        m_table->setRowCount(m_allRows.size() + 1); // + 1 строчка на кнопку добавить
        m_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
        m_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Login"));
        m_table->setHorizontalHeaderItem(2, new QTableWidgetItem(""));
        m_table->setHorizontalHeaderItem(3, new QTableWidgetItem("Pass"));
        m_table->setHorizontalHeaderItem(4, new QTableWidgetItem(""));
        m_table->setHorizontalHeaderItem(5, new QTableWidgetItem("Comment"));
        m_table->setHorizontalHeaderItem(6, new QTableWidgetItem(""));
        fillTableValues();
        bindData();
    }
    void removeItem(int row)
    {
        m_allRows.erase(m_allRows.begin() + row);
        m_changed = true;
        fillTable();
    }
    void addItem()
    {
        m_allRows.push_back(ManagerRow{});
        fillTable();
    }
    QString returnCSV()
    {
        QString result;
        QTextStream stream(&result);
        size_t sz = m_allRows.size();
        for(size_t i = 0; i < sz ; i++)
        {
            stream << m_allRows[i].Name << ";";
            stream << m_allRows[i].Login << ";";
            stream << m_allRows[i].Pass << ";";
            stream << m_allRows[i].Comment << ";";
            stream << "\n";
        }
        return result;
    }
    void readCSV(QString csvText)
    {
        QString tmpStr = "";
        int fieldNumber = 0;
        std::map<int, QString> currFiled;

        for(size_t i = 0; i < csvText.size(); i++)
        {
            QChar curr = csvText[i];
            if (curr == ';')
            {
                currFiled[fieldNumber] = tmpStr;
                tmpStr = "";
                fieldNumber++;
                continue;
            }
            if (curr == '\n')
            {
                m_allRows.push_back(ManagerRow{currFiled[0],currFiled[1],currFiled[2],currFiled[3]});
                fieldNumber = 0;
                currFiled.clear();
                continue;
            }
            tmpStr += curr;
        }
    }
public:
    Manager(QObject* parent, QTableWidget* table) : QObject(parent), m_table(table)
    {

    }
    void fillTable()
    {
        fillTableInner();
    }
    virtual ~Manager()
    {
        qDebug() << "Manager cleaned";
    }
    QByteArray loadFile(QString const& filename)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
            return {};
        return file.readAll();
    }
    void saveFile(const QString &filePath, const QByteArray &data)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            return;
        }

        file.write(data);
    }
    QByteArray encrypt(const QByteArray& data, const QString& keyString)
    {
        QByteArray keyBytes = keyString.toUtf8();
        keyBytes.resize(16);
        struct AES_ctx ctx;
        AES_init_ctx(&ctx, reinterpret_cast<uint8_t*>(keyBytes.data()));
        const int blockSize = 16;
        const int dataSz = data.size();
        QByteArray result;

        for (int i = 0; i < dataSz; i += blockSize)
        {
            QByteArray block = data.mid(i, blockSize);

            if (block.size() < blockSize)
                block.append(blockSize - block.size(), '\0');

            AES_ECB_encrypt(&ctx, reinterpret_cast<uint8_t*>(block.data()));

            result.append(block);
        }
        return result;
    }
    QByteArray decrypt(const QByteArray& data, const QString& keyString)
    {
        QByteArray keyBytes = keyString.toUtf8();
        keyBytes.resize(16);
        struct AES_ctx ctx;
        AES_init_ctx(&ctx, reinterpret_cast<uint8_t*>(keyBytes.data()));
        const int blockSize = 16;
        const int dataSz = data.size();
        QByteArray result;

        for (int i = 0; i < dataSz; i += blockSize)
        {
            QByteArray block = data.mid(i, blockSize);

            if (block.size() < blockSize)
                block.append(blockSize - block.size(), '\0');

            AES_ECB_decrypt(&ctx, reinterpret_cast<uint8_t*>(block.data()));

            result.append(block);
        }
        return result;
    }

public slots:
    void save()
    {
        qDebug() << "Save";
        QString fileName = QFileDialog::getSaveFileName(
            m_table,                               // parent
            tr("Save File"),                       // title
            QString("output.txt"),                 // имя по умолчанию
            tr("Text Files (*.txt);;All Files (*)")
            );

        if (fileName.isEmpty()) {
            return;
        }

        bool ok;
        bool useSafety = true;
        QString textPassword = QInputDialog::getText(
            nullptr,                  // parent
            "Password",             // заголовок
            "Enter the password:",          // сообщение
            QLineEdit::Normal,         // режим ввода
            "",                        // значение по умолчанию
            &ok                        // нажал ли пользователь OK
            );

        if (textPassword.isEmpty() || ok == false) {
            useSafety = false;
        }

        QString csv = returnCSV();
        if (useSafety)
        {
            qDebug() << "ciphering";
            auto tmp = encrypt(csv.toUtf8(), textPassword);
            saveFile(fileName, tmp);
        }
        else
        {
            qDebug() << "no ciphering";
            saveFile(fileName, csv.toUtf8());
        }

    }
    void load()
    {
        qDebug() << "Load";
        QString fileName = QFileDialog::getOpenFileName(
            m_table,                                // parent
            tr("Open File"),                       // title
            QString(),                             // start directory
            tr("Text Files (*.txt);;All Files (*)")
            );

        if (fileName.isEmpty()) {
            return;
        }

        bool ok;
        bool useSafety = true;
        QString textPassword = QInputDialog::getText(
            nullptr,                  // parent
            "Password",             // заголовок
            "Enter the password:",          // сообщение
            QLineEdit::Normal,         // режим ввода
            "",                        // значение по умолчанию
            &ok                        // нажал ли пользователь OK
            );

        if (textPassword.isEmpty() || ok == false) {
            useSafety = false;
        }

        QByteArray array = loadFile(fileName);

        QString str;
        if (useSafety)
        {
            QByteArray tmp = decrypt(array, textPassword);
            str = QString::fromUtf8(tmp);
        }
        else
        {
            str = QString::fromUtf8(array);
        }

        m_allRows.clear();
        readCSV(str);
        fillTable();
    }
};

#endif // LOGIC_H
