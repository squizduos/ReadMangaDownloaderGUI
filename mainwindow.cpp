#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chapter.h"
#include "manga.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->loadLocalChapters();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::getChaptersToList(WebChapter *chapter)
{
    this->notInitializedChapters--;
    if (!this->searchInLocalChapters(chapter))
    {
        this->manga->webChapters.append(chapter);
        if (this->notInitializedChapters <= 0)
        {
            for(int i = 0; i < manga->webChapters.length(); i++)
            {
                for (int j = 0; j < manga->webChapters.length(); j++)
                {
                    if (*manga->webChapters[i] > *manga->webChapters[j])
                    {
                        WebChapter* temp = manga->webChapters[i];
                        manga->webChapters[i] = manga->webChapters[j];
                        manga->webChapters[j] = temp;
                    }
                }
            }
            this->ui->downloadButton->setEnabled(true);
            foreach(WebChapter *newChapter, manga->webChapters)
            {
                this->ui->chaptersList->addItem(QString("Том %1, глава %2, номер %3").arg(newChapter->volume, 3).arg(newChapter->chapter, 3).arg(newChapter->chapterLink.toString()));
            }
            this->ui->statusBar->showMessage("Готово");
            ui->loadButton->setEnabled(true);
            ui->loadUrl->setEnabled(true);
            ui->chaptersList->setEnabled(true);
            ui->downloadButton->setEnabled(true);
        }
    }

}

void MainWindow::setDownloadProgress(int percent)
{
    ui->chapterProgress->setValue(percent);
}

void MainWindow::setDownloadStatus(QString manga, int volume, int chapter)
{
    ui->downloadingStatus->setText(QString("Скачивается: манга %1, том %2, глава %3").arg(manga).arg(volume).arg(chapter));
}

void MainWindow::addToDownloadLog(QUrl url)
{
    ui->downloadingLog->addItem(url.toString());
}

void MainWindow::on_downloadButton_clicked()
{
    if (ui->chaptersList->selectedItems().count() > 0)
    {
        ui->downloadButton->setDisabled(true);
        ui->loadButton->setEnabled(false);
        ui->loadUrl->setEnabled(false);
        ui->chaptersList->setEnabled(false);
        this->ui->statusBar->showMessage("Загружаем главы манги...");
        downloadedChapters = 0;
        notDownloadedChapters = ui->chaptersList->selectedItems().count();
        for (int i = 0; i < ui->chaptersList->count(); i++)
        {
            if (ui->chaptersList->item(i)->isSelected())
            {
                this->downloadingChapters.append(this->manga->webChapters[i]);
            }
        }
        this->downloadingChapters.first()->downloadChapters();
    }
}

void MainWindow::finishDownloadChapter(WebChapter *chapter)
{
    this->loadLocalChapters();
    this->downloadingChapters.removeAll(chapter);
    downloadedChapters += 1;
    this->ui->mangaProgress->setValue((int) ((downloadedChapters * 100 / notDownloadedChapters)));
    if (this->downloadingChapters.empty())
    {
        this->ui->mangaProgress->setValue(0);
        this->ui->chapterProgress->setValue(0);
        ui->downloadButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->loadUrl->setEnabled(true);
        ui->chaptersList->setEnabled(true);
        this->ui->statusBar->showMessage("Готово");
        QMessageBox::information(this, "ReadMangaDownloader", "Главы успешно скачаны!");
    }
    else
    {
        this->downloadingChapters.first()->downloadChapters();
    }

}

void MainWindow::updateManga()
{
    this->ui->mangaInfo->setText(QString("Манга: %1, автор: %2").arg(manga->name).arg(manga->author));
}

void MainWindow::setMangaChaptersCount()
{
    this->notInitializedChapters++;
}

void MainWindow::loadLocalChapters()
{
    this->ui->localList->clear();
    localChapters.clear();
    QDir dir("./");
    QRegExp ifChapterDir("vol([0-9]+)ch([0-9]+)");
    QStringList lstDirs = dir.entryList(QDir::Dirs |
        QDir::AllDirs |
        QDir::NoDotAndDotDot);
    foreach(QString m, lstDirs)
    {
        QDir mangaDir("./" + m);
        QStringList chapterDirsList = mangaDir.entryList(QDir::Dirs |
            QDir::AllDirs |
            QDir::NoDotAndDotDot);
        foreach (QString chapter, chapterDirsList)
        {
            ifChapterDir.indexIn(chapter);
            if (ifChapterDir.cap().length() > 0)
            {
                LocalChapter* localChapter = new LocalChapter(this, QString("./"+m+"/"+chapter));
                this->localChapters.append(localChapter);
            }
        }
    }
    foreach(LocalChapter* l, this->localChapters)
    {
        this->ui->localList->addItem(QString("Манга %1, том %2, глава %3").arg(l->manga).arg(l->volume, 3).arg(l->chapter, 3));
    }
}

void MainWindow::errorLoadingManga(QString error)
{
    delete manga;
    QMessageBox::warning(this, "ReadMangaDownloader", error);
    ui->downloadButton->setEnabled(true);
    ui->loadButton->setEnabled(true);
    ui->loadUrl->setEnabled(true);
    ui->chaptersList->setEnabled(true);
}

void MainWindow::on_loadButton_clicked()
{
    QRegExp isValidUrl("(readmanga\.me|adultmanga\.ru)\/([a-z, A-Z, 0-9, _]+)");
    isValidUrl.indexIn(this->ui->loadUrl->text());
    if (isValidUrl.cap().length() > 0)
    {
        ui->downloadButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        ui->loadUrl->setEnabled(false);
        ui->chaptersList->setEnabled(false);
        this->ui->statusBar->showMessage("Загружаются сведения о манге...");
        this->notInitializedChapters = 0;
        this->manga = new Manga(this, this->ui->loadUrl->text());
    }
    else
    {
        QMessageBox::information(this, "ReadMangaDownloader", "Неправильный URL!");
    }
}

bool MainWindow::searchInLocalChapters(WebChapter* chapter)
{
    foreach(LocalChapter* localChapter, this->localChapters)
    {
        if (localChapter->manga == chapter->manga && localChapter->volume == chapter->volume && localChapter->chapter == chapter->chapter)
        {
            return true;
        }
    }
    return false;
}
