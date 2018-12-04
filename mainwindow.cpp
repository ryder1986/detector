#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cfg=new ConfigManager();
    dev=NULL;
}

MainWindow::~MainWindow()
{
    delete cfg;
    if(dev)
        delete dev;
    delete ui;
}

void MainWindow::on_pushButton_load_clicked()
{
    if(dev)
        delete dev;
    dev=new Device_Pri(cfg->get_config());
    prt(info,"%s",dev->data().str().data());
    show_in_combox();

}

void MainWindow::on_pushButton_addcam_clicked()
{
   // dev->DeviceConfig.add_camera(std::move(Camera_Pri::get_test_camera()));
    dev->DeviceConfig.add_camera(Camera_Pri::get_test_camera());
    show_in_combox();
}

void MainWindow::on_comboBox_list_activated(int index)
{
    prt(info,"%d selected",index);
    camera_index=index;
}

void MainWindow::on_pushButton_del_clicked()
{
    dev->DeviceConfig.del_camera(camera_index);
    show_in_combox();
}
