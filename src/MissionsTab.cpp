#include "MissionsTab.h"

#include "Savegame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

MissionsTab::MissionsTab(Savegame *savegame) : m_savegame(savegame)
{
    loadObjectives();
    loadNames();

    setLayout(new QHBoxLayout);

    m_missionsList = new QListWidget;
    m_progressList = new QListWidget;

    layout()->addWidget(m_missionsList);
    layout()->addWidget(m_progressList);

    connect(savegame, &Savegame::itemsChanged, this, &MissionsTab::load);
    connect(m_missionsList, &QListWidget::itemSelectionChanged, this, &MissionsTab::onMissionSelected);
    connect(m_progressList, &QListWidget::itemChanged, this, &MissionsTab::onObjectiveChanged);
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

    const QString missionId = m_missionsList->currentItem()->text();

    m_progressList->clear();
    const QVector<bool> objectiveStatus = m_savegame->objectivesCompleted(missionId);
    const QStringList &names = m_objectives[missionId.split('.').first()]; // forgot to fetch the full name from the pak, so just skip the thing after the .
    for (int i=0; i<objectiveStatus.count(); i++) {
        QListWidgetItem *objective{};
        if (i < names.count()) {
            objective = new QListWidgetItem(names[i]);
        } else {
            objective = new QListWidgetItem(QStringLiteral("Unknown objective %1").arg(i + 1));
        }
        objective->setCheckState(objectiveStatus[i] ? Qt::Checked : Qt::Unchecked);
        m_progressList->addItem(objective);
    }

    // TODO
//    m_progressList->setEnabled(false);
}

void MissionsTab::onObjectiveChanged(QListWidgetItem *item)
{
    Q_ASSERT(item);

    int index = m_progressList->row(item);
    Q_ASSERT(index >= 0);

    const QString missionId = m_missionsList->currentItem()->text();
    m_savegame->setObjectiveCompleted(missionId, index, item->checkState() == Qt::Checked ? true : false);

    QTimer::singleShot(100, this, &MissionsTab::onMissionSelected);
}

void MissionsTab::loadObjectives()
{
    QFile objectivesFile(":/data/mission-objectives.json");
    if (!objectivesFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << objectivesFile.fileName() << objectivesFile.fileName();
        return;
    }
    const QJsonObject rootObject = QJsonDocument::fromJson(objectivesFile.readAll()).object();
    for (const QString &id : rootObject.keys()) {
        QStringList objectives;
        for (const QJsonValue &objective : rootObject[id].toArray()) {
            objectives.append(objective.toString());
        }

        m_objectives[id] = std::move(objectives);
    }
}

void MissionsTab::loadNames()
{
    QFile namesFile(":/data/mission-names.json");
    if (!namesFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << namesFile.fileName() << namesFile.fileName();
        return;
    }
    const QJsonObject rootObject = QJsonDocument::fromJson(namesFile.readAll()).object();
    for (const QString &id : rootObject.keys()) {
        m_missionNames[id] = rootObject[id].toString();
    }
}
