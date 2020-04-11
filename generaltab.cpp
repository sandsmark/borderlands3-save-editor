#include "generaltab.h"

#include "Savegame.h"
#include "Constants.h"

#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QVBoxLayout>

GeneralTab::GeneralTab(Savegame *savegame, QWidget *parent) :
    QWidget(parent)
{
    setLayout(new QVBoxLayout);

    //////////////////////////
    // Basic properties
    QGroupBox *basicsBox = new QGroupBox(tr("Basics"));
    QFormLayout *basicsLayout = new QFormLayout;
    basicsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    basicsBox->setLayout(basicsLayout);

    m_nameEdit = new QLineEdit;
    basicsLayout->addRow(tr("Name"), m_nameEdit);

    m_levelEdit = new QSpinBox;
    m_levelEdit->setRange(Constants::minLevel, Constants::maxLevel);
    basicsLayout->addRow(tr("Level"), m_levelEdit);

    m_xpEdit = new QSpinBox;
    m_xpEdit->setRange(0, Constants::requiredXp[Constants::maxLevel]);
    m_xpEdit->setSingleStep(1000);
    basicsLayout->addRow(tr("Experience points"), m_xpEdit);

    layout()->addWidget(basicsBox);

    connect(savegame, &Savegame::nameChanged, m_nameEdit, &QLineEdit::setText);
    connect(savegame, &Savegame::levelChanged, m_levelEdit, &QSpinBox::setValue);
    connect(savegame, &Savegame::xpChanged, m_xpEdit, &QSpinBox::setValue);

    connect(m_nameEdit, &QLineEdit::textEdited, savegame, &Savegame::setCharacterName);
    connect(m_levelEdit, SIGNAL(valueChanged(int)), savegame, SLOT(setLevel(int))); // old style connect because fuck qOverload
    connect(m_xpEdit, SIGNAL(valueChanged(int)), savegame, SLOT(setXp(int))); // old style connect because fuck qOverload

    //////////////////////////
    // Economy
    QGroupBox *economyBox = new QGroupBox(tr("Economy"));
    QFormLayout *economyLayout = new QFormLayout;
    economyLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    economyBox->setLayout(economyLayout);

    m_moneyEditor = new QSpinBox;
    m_moneyEditor->setRange(0, 99999999);
    economyLayout->addRow(tr("Money"), m_moneyEditor);

    m_eridiumEditor = new QSpinBox;
    m_eridiumEditor->setRange(0, 99999999);
    economyLayout->addRow(tr("Eridium"), m_eridiumEditor);

    layout()->addWidget(economyBox);


    connect(savegame, &Savegame::moneyChanged, m_moneyEditor, &QSpinBox::setValue);
    connect(savegame, &Savegame::eridiumChanged, m_eridiumEditor, &QSpinBox::setValue);
    connect(m_moneyEditor, SIGNAL(valueChanged(int)), savegame, SLOT(setMoney(int))); // old style connect because fuck qOverload
    connect(m_eridiumEditor, SIGNAL(valueChanged(int)), savegame, SLOT(setEridium(int))); // old style connect because fuck qOverload
}
