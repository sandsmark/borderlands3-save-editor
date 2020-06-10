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
    Savegame *m_savegame;

    QListWidget *m_missionsList;
    QListWidget *m_progressList;
};

#endif // MISSIONSTAB_H
