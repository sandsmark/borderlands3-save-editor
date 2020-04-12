#include "GameSettingsTab.h"

#include "Savegame.h"

#include <QVBoxLayout>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>


GameSettingsTab::GameSettingsTab(Profile *profile, QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout);

    QGroupBox *graphicsBox = new QGroupBox(tr("Graphics"));
    QFormLayout *graphicsLayout = new QFormLayout;
    graphicsBox->setLayout(graphicsLayout);
    graphicsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    m_fovEdit = new QSpinBox;
    m_fovEdit->setRange(90, 180); // Just guessing what the game can handle
    graphicsLayout->addRow("FOV", graphicsBox);

    layout()->addWidget(graphicsBox);

}
