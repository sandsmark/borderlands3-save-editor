#ifndef MISSIONSTAB_H
#define MISSIONSTAB_H

#include <QWidget>

class Savegame;
class QListWidget;
class QListWidgetItem;

class MissionsTab : public QWidget
{
    Q_OBJECT
public:
    explicit MissionsTab(Savegame *savegame);

signals:

private slots:
    void load();
    void onMissionSelected();
    void onObjectiveChanged(QListWidgetItem *item);

private:
    void loadData();

    Savegame *m_savegame;

    QListWidget *m_missionsList;
    QListWidget *m_progressList;

    QHash<QString, QStringList> m_objectives;
    QHash<QString, QHash<QString, QString>> m_objectiveNames;
    QHash<QString, QString> m_missionNames;
    QHash<QString, QStringList> m_objectiveSets;
};

#endif // MISSIONSTAB_H
