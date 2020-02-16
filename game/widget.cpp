#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDesktopServices>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    group = new QButtonGroup(this);
    group->addButton(ui->radioButton_1);
    group->addButton(ui->radioButton_2);
    group->setId(ui->radioButton_1,1);
    group->setId(ui->radioButton_2,2);
    ui->radioButton_1->toggle();
    fileList.clear();
    ui->checkBox->setChecked(true);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_rom_btn_1_clicked()     //打开rom文件夹
{
    romDir = QFileDialog::getExistingDirectory(this,tr("文件对话框"),"");
    if(romDir.isEmpty())
    {
        ui->label_1->clear();
    }
    else
    {
        ui->label_1->setText(romDir+"/");
    }
    //refreshlist(romDir);

}

void Widget::on_list_btn_2_clicked()    //导出游戏列表
{
    if(!refreshlist(romDir))
    {
        return;
    }
    QString fileDir = QFileDialog::getSaveFileName(this,tr("保存对话框"),"",tr("文本文件(*ini *txt)"));
    if(fileDir.isEmpty())
    {
        ui->label_2->clear();
        return;
    }
    ui->label_2->setText(fileDir);
    QFile file(fileDir);

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("文件打开失败    "));
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < fileList.size(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        QString str = fileInfo.fileName();
        str.resize( str.lastIndexOf('.'));
        out << str <<endl;

    }
    file.close();
    QMessageBox::information(this,tr("提示对话框"),tr("已完成       "));
}

void Widget::on_name_btn_3_clicked()    //导出重命名文件
{
    if(!refreshlist(romDir))
    {
        return;
    }
    QString fileDir = QFileDialog::getSaveFileName(this,tr("保存对话框"),"",tr("文本文件(*ini)"));
    if(fileDir.isEmpty())
    {
        ui->label_3->clear();
        return;
    }
    ui->label_3->setText(fileDir);
    QFile file(fileDir);

    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("文件打开失败    "));
        return;
    }

    QTextStream out(&file);
    out << "[Alias]" <<endl;
    for (int i = 0; i < fileList.size(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        QString str = fileInfo.fileName();
        if(ui->checkBox->isChecked()&&(str.indexOf('\'')>-1||str.indexOf('[')==0||str.indexOf('=')>-1))
        {
            continue;
        }
        str.resize( str.lastIndexOf('.'));
        out << str ;
        if(group->checkedId()==2)
        {
            int a = str.indexOf(" - ");
            if(a<0)
            {
                QMessageBox::warning(this,tr("警告对话框"),tr("发现非编号条目，请重新选择列表类型！"));
                ui->label_3->setText("无编号:"+str);
                file.close();
                return;
            }
            QString front = str.mid(0,a);
            str = str.mid(a+3);
            int aaa=str.indexOf(" (");
            if(str.indexOf(" - ")>0)
            {
                aaa=str.indexOf(" (")<str.indexOf(" - ")?str.indexOf(" ("):str.indexOf(" - ");
            }
            if(aaa<1)
            {
                QMessageBox::warning(this,tr("警告对话框"),tr("该游戏列表不满足重命名条件！"));
                ui->label_3->setText("不满足重命名:"+str);
                file.close();
                return;
            }
            str.insert(aaa,"__");
            str =str + " - " + front;
        }
        else if(group->checkedId()==1)
        {
            int aaa=str.indexOf(" (");
            if(str.indexOf(" - ")>0)
            {
                aaa=str.indexOf(" (")<str.indexOf(" - ")?str.indexOf(" ("):str.indexOf(" - ");
            }
            if(aaa<1)
            {
                QMessageBox::warning(this,tr("警告对话框"),tr("该游戏列表不满足重命名条件！"));
                ui->label_3->setText("不满足重命名:"+str);
                file.close();
                return;
            }
            str.insert(aaa,"__");
        }
        else
        {
            file.close();
            return;
        }

        out << " = " << str <<endl;

    }
    file.close();
    QMessageBox::information(this,tr("提示对话框"),tr("已完成       "));
}

void Widget::on_txt_btn_4_clicked()     //批量导出txt文件
{
    if(!refreshlist(romDir))
    {
        return;
    }
    QString fileDir = QFileDialog::getExistingDirectory(this,tr("文件对话框"),"");
    if(fileDir.isEmpty())
    {
        ui->label_4->clear();
        return;
    }
    for (int i = 0; i < fileList.size(); ++i)
    {
        QFileInfo fileInfo = fileList.at(i);
        QString str = fileInfo.fileName();
        if(ui->checkBox->isChecked()&&(str.indexOf('\'')>-1||str.indexOf('[')==0||str.indexOf('=')>-1))
        {
            continue;
        }
        str.resize( str.lastIndexOf('.'));
        if(group->checkedId()==2)
        {
            int a = str.indexOf(" - ");
            if(a < 0)
            {
                QMessageBox::warning(this,tr("警告对话框"),tr("发现非编号条目，请重新选择列表类型！"));
                ui->label_4->setText("无编号:"+str);
                return;
            }
            str = str.mid(a+3);

        }
        else if(group->checkedId()==1)
        {

        }
        else
        {
            QMessageBox::warning(this,tr("警告对话框"),tr("未识别到游戏名称类型！"));
            return;
        }
        int aaa=str.indexOf(" (");
        if(str.indexOf(" - ")>0)
        {
            aaa=str.indexOf(" (")<str.indexOf(" - ")?str.indexOf(" ("):str.indexOf(" - ");
        }
        if(aaa<1)
        {
            QMessageBox::warning(this,tr("警告对话框"),tr("该游戏列表不满足重命名条件！"));
            ui->label_4->setText("不满足重命名:"+str);
            return;
        }
        str.resize(aaa);
        str = fileDir + "/" + str + ".txt";
        QFile file_create(str);
        if(!file_create.open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,tr("警告对话框"),tr("文件无法创建    "));
            return;
        }
        file_create.close();
    }
    ui->label_4->setText(fileDir+"/");
    QMessageBox::information(this,tr("提示对话框"),tr("已完成       "));

}

void Widget::on_openDir_1_clicked()
{
    openDir(ui->label_1->text());
}

void Widget::on_openDir_2_clicked()
{
    openDir(ui->label_2->text());
}

void Widget::on_openDir_3_clicked()
{
    openDir(ui->label_3->text());
}

void Widget::on_openDir_4_clicked()
{
    openDir(ui->label_4->text());
}

void Widget::openDir(QString fileDir)
{
    if(fileDir.lastIndexOf('/')<0)
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("无法打开文件夹   "));
        return;
    }
    fileDir.remove(fileDir.split("/").last());
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileDir));
}

bool Widget::refreshlist(QString romDir)
{
    if(romDir.isEmpty())
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("未找到目录！    "));
        ui->label_1->setText(tr("请先打开rom目录"));
        return false;
    }
    QDir dir(romDir);
    fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    if(fileList.empty())
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("目录为空      "));
        return false;
    }
    return true;
}
