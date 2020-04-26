#include "ConsumablesTab.h"
#include "Savegame.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>

ConsumablesTab::ConsumablesTab(Savegame *savegame) :
    m_savegame(savegame)
{
    setLayout(new QVBoxLayout);

    QGroupBox *ammoBox = new QGroupBox(tr("Ammo"));
    layout()->addWidget(ammoBox);

    QFormLayout *ammoLayout = new QFormLayout;
    ammoLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    ammoBox->setLayout(ammoLayout);

    ////////// Yes, I could do this nicer and just store everything in a map
    /// But I like doing it the cumbersome way. Or something.

    m_rifleAmmo = new QSpinBox;
    m_rifleAmmo->setMaximum(1400);

    m_smgAmmo = new QSpinBox;
    m_smgAmmo->setMaximum(1800);

    m_heavyAmmo = new QSpinBox;
    m_heavyAmmo->setMaximum(36);

    m_pistolAmmo = new QSpinBox;
    m_pistolAmmo->setMaximum(1000);

    m_sniperAmmo = new QSpinBox;
    m_sniperAmmo->setMaximum(144);

    m_grenadeAmmo = new QSpinBox;
    m_grenadeAmmo->setMaximum(11);

    m_shotgunAmmo = new QSpinBox;
    m_shotgunAmmo->setMaximum(240);

    ammoLayout->addRow(tr("Pistol"), m_pistolAmmo);
    ammoLayout->addRow(tr("SMG"), m_smgAmmo);
    ammoLayout->addRow(tr("Assault Rifle"), m_rifleAmmo);
    ammoLayout->addRow(tr("Shotgun"), m_shotgunAmmo);
    ammoLayout->addRow(tr("Sniper"), m_sniperAmmo);
    ammoLayout->addRow(tr("Heavy"), m_heavyAmmo);
    ammoLayout->addRow(tr("Grenade"), m_grenadeAmmo);

    QGroupBox *sduBox = new QGroupBox(tr("SDU"));
    layout()->addWidget(sduBox);
    QFormLayout *sduLayout = new QFormLayout;
    sduBox->setLayout(sduLayout);

    m_rifleSdu = new QSpinBox;
    m_rifleSdu->setMaximum(8);

    m_smgSdu = new QSpinBox;
    m_smgSdu->setMaximum(8);

    m_heavySdu = new QSpinBox;
    m_heavySdu->setMaximum(8);

    m_pistolSdu = new QSpinBox;
    m_pistolSdu->setMaximum(8);

    m_sniperSdu = new QSpinBox;
    m_sniperSdu->setMaximum(8);

    m_grenadeSdu = new QSpinBox;
    m_grenadeSdu->setMaximum(8);

    m_shotgunSdu = new QSpinBox;
    m_shotgunSdu->setMaximum(8);

    m_backpackSdu = new QSpinBox;
    m_backpackSdu->setMaximum(8);

    sduLayout->addRow(tr("Pistol"), m_pistolSdu);
    sduLayout->addRow(tr("SMG"), m_smgSdu);
    sduLayout->addRow(tr("Assault Rifle"), m_rifleSdu);
    sduLayout->addRow(tr("Shotgun"), m_shotgunSdu);
    sduLayout->addRow(tr("Sniper"), m_sniperSdu);
    sduLayout->addRow(tr("Heavy"), m_heavySdu);
    sduLayout->addRow(tr("Grenade"), m_grenadeSdu);
    sduLayout->addRow(tr("Backpack"), m_backpackSdu);

    connect(m_savegame, &Savegame::fileLoaded, this, &ConsumablesTab::load);
}

void ConsumablesTab::load()
{
    disconnectSpinBoxes();

    m_pistolAmmo->setValue(m_savegame->ammoAmount("Pistol"));
    m_smgAmmo->setValue(m_savegame->ammoAmount("SMG"));
    m_rifleAmmo->setValue(m_savegame->ammoAmount("AssaultRifle"));
    m_sniperAmmo->setValue(m_savegame->ammoAmount("Sniper"));
    m_heavyAmmo->setValue(m_savegame->ammoAmount("Heavy"));
    m_shotgunAmmo->setValue(m_savegame->ammoAmount("Shotgun"));
    m_grenadeAmmo->setValue(m_savegame->ammoAmount("Grenade"));

    m_pistolSdu->setValue(m_savegame->sduAmount("Pistol"));
    m_smgSdu->setValue(m_savegame->sduAmount("SMG"));
    m_rifleSdu->setValue(m_savegame->sduAmount("AssaultRifle"));
    m_sniperSdu->setValue(m_savegame->sduAmount("Sniper"));
    m_heavySdu->setValue(m_savegame->sduAmount("Heavy"));
    m_shotgunSdu->setValue(m_savegame->sduAmount("Shotgun"));
    m_grenadeSdu->setValue(m_savegame->sduAmount("Grenade"));
    m_backpackSdu->setValue(m_savegame->sduAmount("Backpack"));

    connectSpinBoxes();
}

void ConsumablesTab::connectSpinBoxes()
{
    // Fuckings to qt
    connect(m_pistolAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("Pistol", amount);
    });

    connect(m_smgAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("SMG", amount);
    });

    connect(m_rifleAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("AssaultRifle", amount);
    });

    connect(m_shotgunAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("Shotgun", amount);
    });

    connect(m_sniperAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("Sniper", amount);
    });

    connect(m_heavyAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("Heavy", amount);
    });

    connect(m_grenadeAmmo, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setAmmoAmount("Grenade", amount);
    });

    // Fuckings to qt
    connect(m_pistolSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Pistol", amount);
    });

    connect(m_smgSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("SMG", amount);
    });

    connect(m_rifleSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("AssaultRifle", amount);
    });

    connect(m_shotgunSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Shotgun", amount);
    });

    connect(m_sniperSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Sniper", amount);
    });

    connect(m_heavySdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Heavy", amount);
    });

    connect(m_grenadeSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Grenade", amount);
    });

    connect(m_backpackSdu, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int amount) {
        m_savegame->setSduAmount("Backpack", amount);
    });
}

void ConsumablesTab::disconnectSpinBoxes()
{
    // fuckings qspinbox........................
    disconnect(m_pistolAmmo, nullptr, this, nullptr);
    disconnect(m_smgAmmo, nullptr, this, nullptr);
    disconnect(m_rifleAmmo, nullptr, this, nullptr);
    disconnect(m_sniperAmmo, nullptr, this, nullptr);
    disconnect(m_heavyAmmo, nullptr, this, nullptr);
    disconnect(m_shotgunAmmo, nullptr, this, nullptr);
    disconnect(m_grenadeAmmo, nullptr, this, nullptr);

    // fuckings qspinbox........................
    disconnect(m_pistolSdu, nullptr, this, nullptr);
    disconnect(m_smgSdu, nullptr, this, nullptr);
    disconnect(m_rifleSdu, nullptr, this, nullptr);
    disconnect(m_sniperSdu, nullptr, this, nullptr);
    disconnect(m_heavySdu, nullptr, this, nullptr);
    disconnect(m_shotgunSdu, nullptr, this, nullptr);
    disconnect(m_grenadeSdu, nullptr, this, nullptr);
    disconnect(m_backpackSdu, nullptr, this, nullptr);
}
