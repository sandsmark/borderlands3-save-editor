#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "Savegame.h"

class QLineEdit;
class QPushButton;
class QSpinBox;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void openFile();
    void saveFile();

private:
    Savegame m_savegame;
    QLineEdit *m_nameEdit;
    QSpinBox *m_levelEdit;
    QPushButton *m_saveButton;

//    std::unique_ptr
};
#endif // WIDGET_H
