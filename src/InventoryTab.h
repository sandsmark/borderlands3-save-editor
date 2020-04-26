#ifndef INVENTORYTAB_H
#define INVENTORYTAB_H

#include <QWidget>

class QListWidget;
class Savegame;

class InventoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit InventoryTab(Savegame *savegame, QWidget *parent = nullptr);

private slots:
    void onSelected();
    void load();

private:
    Savegame *m_savegame;
    QListWidget *m_list;
    QListWidget *m_partsList;
};

#endif // INVENTORYTAB_H
