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
    loadData();

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
    for (const QString &missionPath : m_savegame->activeMissions()) {
        const QString mission = missionPath.split('.').first(); // forgot to fetch the full name from the pak, so just skip the thing after the .
        QListWidgetItem *item;
        if (m_missionNames.contains(mission)) {
            item = new QListWidgetItem(m_missionNames[mission]);
        } else {
            item = new QListWidgetItem(mission);
        }
        item->setData(Qt::UserRole, missionPath);

        m_missionsList->addItem(item);
    }
}

void MissionsTab::onMissionSelected()
{
    if (!m_missionsList->currentItem()) {
        return;
    }

    const QString missionId = m_missionsList->currentItem()->data(Qt::UserRole).toString();
    qDebug() << missionId;

    m_progressList->clear();
    bool failed;

    const QVector<bool> objectiveStatus = m_savegame->objectivesCompleted(missionId, &failed);
    const QString missionLookupName = missionId.split('.').first();
    const QStringList &objectivePaths = m_objectives[missionLookupName];
    const QHash<QString, QString> &objectiveNames = m_objectiveNames[missionLookupName];

    for (int i=0; i<objectiveStatus.count(); i++) {
        QListWidgetItem *objective{};
        if (i < objectivePaths.count()) {
            if (objectiveNames.contains(objectivePaths[i])) {
                objective = new QListWidgetItem(objectiveNames[objectivePaths[i]]);
            } else {
                objective = new QListWidgetItem(objectivePaths[i]);
            }
            objective->setData(Qt::UserRole, objectivePaths[i]);
        } else {
            objective = new QListWidgetItem(QStringLiteral("Unknown objective %1").arg(i + 1));
            failed = true;
        }

        objective->setCheckState(objectiveStatus[i] ? Qt::Checked : Qt::Unchecked);
        m_progressList->addItem(objective);
    }

    m_progressList->setEnabled(!failed);
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

void MissionsTab::loadData()
{
    QFile namesFile(":/data/missions.json");
    if (!namesFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << namesFile.fileName() << namesFile.fileName();
        return;
    }
    const QJsonObject rootObject = QJsonDocument::fromJson(namesFile.readAll()).object();
    for (const QString &missionPath : rootObject.keys()) {
        const QJsonObject mission = rootObject[missionPath].toObject();
        m_missionNames[missionPath] = mission["Title"].toString();
        for (const QJsonValue &val : mission["ObjectivesList"].toArray()) {
            m_objectives[missionPath].append(val.toString());
        }
        const QJsonObject objectiveTitles = mission["ObjectiveTitles"].toObject();
        for (const QString &objectivePath : objectiveTitles.keys()) {
            m_objectiveNames[missionPath][objectivePath] = objectiveTitles[objectivePath].toString();
        }
    }
}
