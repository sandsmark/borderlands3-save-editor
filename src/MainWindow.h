#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>

class GeneralTab;
class Savegame;

class QPushButton;
class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setFilePath(const QString &path) { m_filePath = path; }

private slots:
    void onOpenFile();
    void onSaveFile();
    void onSaveAs();

    void loadFile();

private:
    Savegame *m_savegame;
    QString m_filePath;
    QTabWidget *m_tabWidget;
    GeneralTab *m_generalTab;
};
#endif // WIDGET_H
