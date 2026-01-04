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
    void bindData();
    void placeEdit(int row, int column, const QString& text);
    void activated(int row, int column);
    void changed(QTableWidgetItem *current, QTableWidgetItem *previous);
    void copy(int row, int column);
    void fillTableValues();
    void fillTableInner();
    void removeItem(int row);
    void addItem();
    QString returnCSV();
    void readCSV(QString csvText);
public:
    Manager(QObject* parent, QTableWidget* table) : QObject(parent), m_table(table)
    {

    }
    void fillTable();
    virtual ~Manager()
    {
        qDebug() << "Manager cleaned";
    }
    QByteArray loadFile(QString const& filename);
    void saveFile(const QString &filePath, const QByteArray &data);
    QByteArray convertStringToArray(const QString& keyString);
    QByteArray encrypt(const QByteArray& data, const QString& keyString);
    QByteArray decrypt(const QByteArray& data, const QString& keyString);

public slots:
    void save();
    void load();
};

#endif // LOGIC_H
