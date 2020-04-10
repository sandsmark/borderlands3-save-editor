#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "Savegame.h"

class QLineEdit;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void openFile();

private:
    Savegame m_savegame;
    QLineEdit *m_nameEdit;

//    std::unique_ptr
};
#endif // WIDGET_H
