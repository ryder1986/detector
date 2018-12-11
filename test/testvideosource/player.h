#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <iostream>
#include <string>
class Player : public QWidget
{
    Q_OBJECT
public:
    explicit Player(std::string url,QWidget *parent = 0) : QWidget(parent)
    {

    }

signals:

public slots:
};

#endif // PLAYER_H
