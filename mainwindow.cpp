#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logic.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Manager* m_man = new Manager(this, ui->tableWidget);
    m_man->fillTable();

    connect(ui->actionOpen, &QAction::triggered, m_man, &Manager::load);
    connect(ui->actionSave, &QAction::triggered, m_man, &Manager::save);
}

MainWindow::~MainWindow()
{
    delete ui;
}
