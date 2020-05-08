#ifndef INVENTORYTAB_H
#define INVENTORYTAB_H

#include <QWidget>

class QListWidget;
class Savegame;
class QVBoxLayout;
class QLabel;

class InventoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit InventoryTab(Savegame *savegame, QWidget *parent = nullptr);

private slots:
    void onItemSelected();
    void onPartSelected();
    void load();

private:
    Savegame *m_savegame;
    QListWidget *m_list;
    QListWidget *m_partsList;

    QLabel *m_partName;
    QLabel *m_partEffects;
    QLabel *m_partPositives;
    QLabel *m_partNegatives;
};

#endif // INVENTORYTAB_H
