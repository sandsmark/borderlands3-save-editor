#include "MainWindow.h"

#include "GeneralTab.h"
#include "Savegame.h"
#include "InventoryTab.h"
#include "ConsumablesTab.h"
#include "MissionsTab.h"

#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QSettings>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_savegame(new Savegame(this))
{
    setWindowFlag(Qt::Dialog);

    QToolBar *mainToolbar = addToolBar(tr("Main"));
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    mainToolbar->addAction(QIcon::fromTheme("document-open"), tr("Open..."), this, &MainWindow::onOpenFile);
    mainToolbar->addAction(QIcon::fromTheme("document-save"), tr("Save"), this, &MainWindow::onSaveFile);
    mainToolbar->addAction(QIcon::fromTheme("document-save-as"), tr("Save as..."), this, &MainWindow::onSaveAs);

    // Set up tabs
    m_tabWidget = new QTabWidget;
    setCentralWidget(m_tabWidget);

    m_generalTab = new GeneralTab(m_savegame);
    m_generalTab->setEnabled(false);
    m_tabWidget->addTab(m_generalTab, tr("General"));

    m_inventoryTab = new InventoryTab(m_savegame);
    m_inventoryTab->setEnabled(false);
    m_tabWidget->addTab(m_inventoryTab, tr("Inventory"));

    m_consumablesTab = new ConsumablesTab(m_savegame);
    m_consumablesTab->setEnabled(false);
    m_tabWidget->addTab(m_consumablesTab, "Consumables");

    m_missionsTab = new MissionsTab(m_savegame);
    m_missionsTab->setEnabled(false);
    m_tabWidget->addTab(m_missionsTab, tr("Missions"));

    resize(900, 500);

    QSettings settings;
    if (settings.contains("lastopened")) {
        m_filePath = settings.value("lastopened").toString();

        // Wait until mainloop started
        QMetaObject::invokeMethod(this, "loadFile", Qt::QueuedConnection);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadFile()
{
    QMessageBox::warning(this, tr("Untested warning"), tr("This isn't really well tested. Especially the item editing is completely untested and will probably fuck up something.\nMake backups before using."));
    qDebug() << "Loading" << m_filePath;
    m_generalTab->setEnabled(false);
    m_inventoryTab->setEnabled(false);
    m_consumablesTab->setEnabled(false);
    m_missionsTab->setEnabled(false);
    if (m_filePath.isEmpty()) {
        m_filePath = QFileDialog::getOpenFileName(this, "Select a savegame", QString(), "Savefile (*.sav)");
        if (m_filePath.isEmpty()) {
            return;
        }
    }

    if (!m_savegame->load(m_filePath)) {
        return;
    }
    QSettings settings;
    settings.setValue("lastopened", m_filePath);
    m_generalTab->setEnabled(true);
    m_inventoryTab->setEnabled(true);
    m_consumablesTab->setEnabled(true);
    m_missionsTab->setEnabled(true);
}

void MainWindow::onOpenFile()
{
    QString newPath = QFileDialog::getOpenFileName(this, "Select a savegame", QString(), "Savefile (*.sav)");
    if (newPath.isEmpty()) {
        return;
    }
    m_filePath = newPath;

    loadFile();
}

void MainWindow::onSaveFile()
{
    if (!m_savegame->save(m_filePath)) {
        return;
    }

    QSettings settings;
    settings.setValue("lastopened", m_filePath);
}

void MainWindow::onSaveAs()
{
    QString newPath = QFileDialog::getSaveFileName(this, "Select a filename to save to", QString(), "Savefile (*.sav)");
    if (newPath.isEmpty()) {
        return;
    }

    m_filePath = newPath; // so we save to this next time the save button is clicked
    if (!m_savegame->save(m_filePath)) {
        qWarning() << "Failed to save";
        return;
    }
}

