#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <QListWidgetItem>

typedef unsigned long long ui64;
typedef unsigned char bit;

ui64 file_size(const char *file)
{
    struct stat info;
    memset(&info, 0, sizeof(struct stat));
    stat(file, &info);
    return static_cast<ui64>(info.st_size);
}

char *read_string(FILE *f)
{
    ui64 i;
    char *s = static_cast<char *>(malloc(sizeof(char) * 128));
    fread(&i, sizeof(ui64), 1, f);
    fread(s, sizeof(char), i, f);
    s[i] = '\0';
    return s;
}


ui64 read_ui64(FILE *f)
{
    ui64 i;
    fread(&i, sizeof(ui64), 1, f);
    return i;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::ListMode);
    connect(ui->Select, SIGNAL(clicked()), this, SLOT(get_file()));
    connect(ui->Quit, SIGNAL(clicked()), this, SLOT(quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_file()
{
    ui->listWidget->clear();
    QString file = QFileDialog::getOpenFileName();
    FILE *fp = fopen(file.toStdString().c_str(), "rb");
    ui64 size = file_size(file.toStdString().c_str());

    QListWidgetItem *itm;

    itm = new QListWidgetItem;
    itm->setText("Software: "+ tr(read_string(fp)));
    ui->listWidget->addItem(itm);

    itm = new QListWidgetItem;
    itm->setText("Version: "+ tr(read_string(fp)));
    ui->listWidget->addItem(itm);

    itm = new QListWidgetItem;
    itm->setText("Author: "+ tr(read_string(fp)));
    ui->listWidget->addItem(itm);

    itm = new QListWidgetItem;
    itm->setText(tr(""));
    ui->listWidget->addItem(itm);

    itm = new QListWidgetItem;
    itm->setText(tr("------------------------------------------------"));
    ui->listWidget->addItem(itm);

    itm = new QListWidgetItem;
    itm->setText(tr(""));
    ui->listWidget->addItem(itm);

    while (true) {
        QListWidgetItem *itm = new QListWidgetItem;
        QString file = tr(read_string(fp));
        ui64 skip = read_ui64(fp);
        char tmp[32];
        sprintf(tmp, "%lu", skip);
        QString siz = tr(tmp);
        itm->setText(file + "    ->    " + siz + " bits");
        ui->listWidget->addItem(itm);

        fseek(fp, static_cast<long>(skip), SEEK_CUR);

        if (static_cast<ui64>(ftell(fp)) >= size)
            break;
    }
}

void MainWindow::quit()
{
    exit(0);
}
