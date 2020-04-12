#ifndef GAMESETTINGSTAB_H
#define GAMESETTINGSTAB_H

#include <QWidget>

class QSpinBox;
class Profile;

class GameSettingsTab : public QWidget
{
    Q_OBJECT
public:
    explicit GameSettingsTab(Profile *savegame, QWidget *parent = nullptr);

signals:

private:
    QSpinBox *m_fovEdit;


};

#endif // GAMESETTINGSTAB_H
