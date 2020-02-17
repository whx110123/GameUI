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

    menulabel = new QMenu(this);
    act1label = new QAction(tr("打开目录"),this);
    actindex = 0;
    menulabel->addAction(act1label);
    ui->label_1->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->label_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->label_3->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->label_4->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->label_1,&QLabel::customContextMenuRequested,[=]()
    {
        actindex = 1;
        menulabel->exec(QCursor::pos());
    });
    connect(ui->label_2,&QLabel::customContextMenuRequested,[=]()
    {
        actindex = 2;
        menulabel->exec(QCursor::pos());
    });
    connect(ui->label_3,&QLabel::customContextMenuRequested,[=]()
    {
        actindex = 3;
        menulabel->exec(QCursor::pos());
    });
    connect(ui->label_4,&QLabel::customContextMenuRequested,[=]()
    {
        actindex = 4;
        menulabel->exec(QCursor::pos());
    });
    connect(act1label,&QAction::triggered,[=]()
    {
        switch (actindex)
        {
        case 1:
            openDir(ui->label_1->text());
            break;
        case 2:
            openDir(ui->label_2->text());
            break;
        case 3:
            openDir(ui->label_3->text());
            break;
        case 4:
            openDir(ui->label_4->text());
            break;
        default:
            QMessageBox::warning(this,tr("警告对话框"),tr("未找到目录！    "));
            break;
        }
    });

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_rom_btn_1_clicked()     //还原所有路径
{

    ui->label_1->setText(tr("文件夹路径"));
    ui->label_2->setText(tr("文件路径"));
    ui->label_3->setText(tr("文件路径"));
    ui->label_4->setText(tr("文件夹路径"));
    ui->label_5->setText(tr("提醒："));
    romDir.clear();

}

void Widget::on_list_btn_2_clicked()    //导出游戏列表
{
    if(!refreshlist())
    {
        return;
    }
    QString fileDir = ui->label_2->text();
    if(fileDir.lastIndexOf('/')<0)
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("路径不正确   "));
        return;
    }
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
    ui->label_5->setText(tr("提醒："));
}

void Widget::on_name_btn_3_clicked()    //导出重命名文件
{
    if(!refreshlist())
    {
        return;
    }

    QString fileDir = ui->label_3->text();
    if(fileDir.lastIndexOf('/')<0)
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("路径不正确   "));
        return;
    }
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
                ui->label_5->setText("提醒：无编号 "+str);
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
                ui->label_5->setText("提醒：不满足重命名 "+str);
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
                ui->label_5->setText("提醒：不满足重命名 "+str);
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
    ui->label_5->setText(tr("提醒："));
}

void Widget::on_txt_btn_4_clicked()     //批量导出txt文件
{
    if(!refreshlist())
    {
        return;
    }
    QString fileDir = ui->label_4->text();
    if(fileDir.lastIndexOf('/')<0)
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("路径不正确   "));
        return;
    }
    QDir dir(fileDir);
    if(!dir.exists())
    {
        int ret = QMessageBox::question(this,tr("文件夹对话框"),tr("文件夹不存在，是否创建   "),QMessageBox::Yes,QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
            dir.mkpath(fileDir);//创建多级目录
        }
        else
        {
            return;
        }
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
                ui->label_5->setText("提醒：无编号 "+str);
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
            ui->label_5->setText("提醒：不满足重命名 "+str);
            return;
        }
        str.resize(aaa);
        str = fileDir  + str + ".txt";
        QFile file_create(str);
        if(!file_create.open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,tr("警告对话框"),tr("文件无法创建    "));
            return;
        }
        file_create.close();
    }

    QMessageBox::information(this,tr("提示对话框"),tr("已完成       "));
    ui->label_5->setText(tr("提醒："));

}

void Widget::on_openDir_1_clicked()     //打开rom文件夹
{
    QString fileDir = QFileDialog::getExistingDirectory(this,tr("文件对话框"),"");
    if(!fileDir.isEmpty())
    {
        romDir = fileDir;
        fileDir.remove(fileDir.split("/").last());
        ui->label_1->setText(romDir+"/");
        ui->label_2->setText(fileDir+"list.ini");
        ui->label_3->setText(fileDir+"alias.ini");
        ui->label_4->setText(fileDir+tr("整合/"));
        ui->label_5->setText(tr("提醒："));
    }
}

void Widget::on_openDir_2_clicked()     //选择列表文件
{
    QString fileDir = QFileDialog::getSaveFileName(this,tr("保存对话框"),"",tr("文本文件(*ini *txt)"));
    if(!fileDir.isEmpty())
    {
        ui->label_2->setText(fileDir);
        ui->label_5->setText(tr("提醒："));
    }
}

void Widget::on_openDir_3_clicked()     //选择命名文件
{
    QString fileDir = QFileDialog::getSaveFileName(this,tr("保存对话框"),"",tr("文本文件(*ini)"));
    if(!fileDir.isEmpty())
    {
        ui->label_3->setText(fileDir);
        ui->label_5->setText(tr("提醒："));
    }
}

void Widget::on_openDir_4_clicked()     //选择txt文件夹
{
    QString fileDir = QFileDialog::getExistingDirectory(this,tr("文件对话框"),"");
    if(!fileDir.isEmpty())
    {
        ui->label_4->setText(fileDir+"/");
        ui->label_5->setText(tr("提醒："));
    }
}

void Widget::openDir(QString fileDir)   //打开指定文件夹
{
    if(fileDir.lastIndexOf('/')<0)
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("无法打开文件夹   "));
        return;
    }
    fileDir.remove(fileDir.split("/").last());
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileDir));
}

bool Widget::refreshlist()              //刷新游戏列表
{
    if(romDir.isEmpty())
    {
        QMessageBox::warning(this,tr("警告对话框"),tr("未找到目录！    "));
        ui->label_5->setText(tr("提醒：请先打开rom目录"));
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
