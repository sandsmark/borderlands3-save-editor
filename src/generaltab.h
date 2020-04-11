#ifndef GENERALTAB_H
#define GENERALTAB_H

#include <QObject>
#include <QWidget>
#include <memory>

class Savegame;

class QLineEdit;
class QSpinBox;

class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(Savegame *savegame, QWidget *parent = nullptr);

signals:


private:
    QLineEdit *m_nameEdit;
    QSpinBox *m_levelEdit;
    QSpinBox *m_xpEdit;
    Savegame *m_savegame;

    QSpinBox *m_moneyEditor;
    QSpinBox *m_eridiumEditor;
};

#endif // GENERALTAB_H
