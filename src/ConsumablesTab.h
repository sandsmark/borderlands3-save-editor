#ifndef CONSUMABLESTAB_H
#define CONSUMABLESTAB_H

#include <QWidget>

class Savegame;
class QSpinBox;

class ConsumablesTab : public QWidget
{
    Q_OBJECT

public:
    ConsumablesTab(Savegame *savegame);

private slots:
    void load();

private:
    void connectSpinBoxes();
    void disconnectSpinBoxes();

    Savegame *m_savegame;

    QSpinBox *m_rifleAmmo;
    QSpinBox *m_grenadeAmmo;
    QSpinBox *m_heavyAmmo;
    QSpinBox *m_pistolAmmo;
    QSpinBox *m_smgAmmo;
    QSpinBox *m_shotgunAmmo;
    QSpinBox *m_sniperAmmo;

    QSpinBox *m_rifleSdu;
    QSpinBox *m_grenadeSdu;
    QSpinBox *m_heavySdu;
    QSpinBox *m_pistolSdu;
    QSpinBox *m_smgSdu;
    QSpinBox *m_shotgunSdu;
    QSpinBox *m_sniperSdu;
    QSpinBox *m_backpackSdu;
};

#endif // CONSUMABLESTAB_H
