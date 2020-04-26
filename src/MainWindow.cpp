#include "MainWindow.h"

#include "generaltab.h"
#include "Savegame.h"
#include "InventoryTab.h"

#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QSettings>



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

    resize(600, 500);

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
    qDebug() << "Loading" << m_filePath;
    m_generalTab->setEnabled(false);
    m_inventoryTab->setEnabled(false);
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
}

