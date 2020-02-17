#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QFileInfoList>
#include <QAction>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_rom_btn_1_clicked();

    void on_list_btn_2_clicked();

    void on_name_btn_3_clicked();

    void on_txt_btn_4_clicked();

    void on_openDir_1_clicked();

    void on_openDir_2_clicked();

    void on_openDir_3_clicked();

    void on_openDir_4_clicked();

private:
    Ui::Widget *ui;
    QButtonGroup *group;
    QFileInfoList fileList;
    QString romDir;
    void openDir(QString fileDir);
    bool refreshlist();

private:
    QMenu *menulabel;
    QAction *act1label;
    int actindex;

};
#endif // WIDGET_H
