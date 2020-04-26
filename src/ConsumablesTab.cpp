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
}
