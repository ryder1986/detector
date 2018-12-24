#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"


#include "videosource.h"
#include "player.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
    {
        wgt=nullptr;
        ui->setupUi(this);
        ui->widget_video->setLayout(&lo);
    }
    ~MainWindow()
    {
        delete ui;
    }

private slots:
    void on_pushButton_open_clicked()
    {
       // p_src=new VideoSource("rtsp://192.168.1.95:554/av0_1");
        if(wgt){
            lo.removeWidget(wgt);
            delete wgt;
        }

        wgt=new Player(ui->lineEdit_url->text().toStdString());
        lo.addWidget(wgt);
       // lo.addWidget(new PlayerWidget(ui->));
     }

private:
    Ui::MainWindow *ui;
    VideoSource *p_src;
    QHBoxLayout lo;
    QWidget *wgt;
};

#endif // MAINWINDOW_H
