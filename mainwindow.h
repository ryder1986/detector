#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configmanager.h"
#include "device_manager.h"
#include "ui_mainwindow.h"

#include <QGridLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_load_clicked();

    void on_pushButton_addcam_clicked();

    void on_comboBox_list_activated(int index);

    void on_pushButton_del_clicked();

    void on_pushButton_play_clicked();

private:
    void show_in_combox()
    {
        ui->comboBox_list->clear();
           ui->comboBox_list->addItem("");
        for(Camera_Pri &p:dev->DeviceConfig.CameraData){
            ui->comboBox_list->addItem(p.Url.data());
        }
    }

private:
    Ui::MainWindow *ui;
    ConfigManager *cfg;
    Device_Pri *dev;
    int camera_index;
    QGridLayout *video_layout;
};

#endif // MAINWINDOW_H
