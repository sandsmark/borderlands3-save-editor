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
    void onEdit();

private:
    void load();

    Savegame *m_savegame;
    QListWidget *m_list;
};

#endif // INVENTORYTAB_H
