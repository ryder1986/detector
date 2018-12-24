#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <iostream>
#include <string>
#include <QOpenGLWidget>
class Player : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Player(std::string url,QWidget *parent = 0)
    {

    }

signals:

public slots:
};

#endif // PLAYER_H
