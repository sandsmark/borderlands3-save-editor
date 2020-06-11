#ifndef MISSIONSTAB_H
#define MISSIONSTAB_H

#include <QWidget>

class Savegame;
class QListWidget;

class MissionsTab : public QWidget
{
    Q_OBJECT
public:
    explicit MissionsTab(Savegame *savegame);

signals:

private slots:
    void load();
    void onMissionSelected();

private:
    void loadObjectives();
    void loadNames();

    Savegame *m_savegame;

    QListWidget *m_missionsList;
    QListWidget *m_progressList;
    QHash<QString, QStringList> m_objectives;
    QHash<QString, QString> m_missionNames;
};

#endif // MISSIONSTAB_H
