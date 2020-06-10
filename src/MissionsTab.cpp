#include "MissionsTab.h"

#include "Savegame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>

MissionsTab::MissionsTab(Savegame *savegame) : m_savegame(savegame)
{
    setLayout(new QHBoxLayout);

    m_missionsList = new QListWidget;
    m_progressList = new QListWidget;

    layout()->addWidget(m_missionsList);
    layout()->addWidget(m_progressList);

    connect(savegame, &Savegame::itemsChanged, this, &MissionsTab::load);
    connect(m_missionsList, &QListWidget::itemSelectionChanged, this, &MissionsTab::onMissionSelected);
}

void MissionsTab::load()
{
    m_missionsList->clear();
    m_missionsList->addItems(m_savegame->activeMissions());
}

void MissionsTab::onMissionSelected()
{
    if (!m_missionsList->currentItem()) {
        return;
    }

    m_progressList->clear();
    const QVector<bool> objectiveStatus = m_savegame->objectivesCompleted(m_missionsList->currentItem()->text());
    for (int i=0; i<objectiveStatus.count(); i++) {
        QListWidgetItem *objective = new QListWidgetItem(QStringLiteral("Objective %1 (TODO lookup name)").arg(i + 1));
        objective->setCheckState(objectiveStatus[i] ? Qt::Checked : Qt::Unchecked);
        m_progressList->addItem(objective);
    }

    // TODO
    m_progressList->setEnabled(false);
}
