#ifndef INVENTORYTAB_H
#define INVENTORYTAB_H

#include <QWidget>
#include <QHash>

class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
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
    void onPartChanged(QTreeWidgetItem *item, int column);
    void load();

private:
    void checkBounds();

    Savegame *m_savegame;
    QListWidget *m_list;
    QTreeWidget *m_partsList;
    QHash<QString, int> m_enabledParts;

    QLabel *m_partName;
    QLabel *m_partEffects;
    QLabel *m_partPositives;
    QLabel *m_partNegatives;
    QLabel *m_warningText;
    int m_selectedInventoryItem = -1;
};

#endif // INVENTORYTAB_H
