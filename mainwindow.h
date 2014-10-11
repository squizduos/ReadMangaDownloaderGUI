#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMessageBox>
#include <QDesktopServices>
#include "chapter.h"
#include "manga.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Manga* manga;
    QList<WebChapter*> downloadingChapters;
    int notInitializedChapters;
    int notDownloadedChapters, downloadedChapters;
    void loadLocalChapters();
    QList<LocalChapter *> localChapters;
    bool searchInLocalChapters(WebChapter* chapter);

public slots:
    void getChaptersToList(WebChapter *chapter);
    void setDownloadProgress(int percent);
    void setDownloadStatus(QString manga, int volume, int chapter);
    void addToDownloadLog(QUrl url);
    void finishDownloadChapter(WebChapter *chapter);
    void updateManga();
    void setMangaChaptersCount();
    void errorLoadingManga(QString error);

private slots:
    void on_downloadButton_clicked();
    void on_loadButton_clicked();
};

#endif // MAINWINDOW_H
