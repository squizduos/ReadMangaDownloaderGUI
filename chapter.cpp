#include "chapter.h"

Chapter::Chapter(QObject *parent) :
    QObject(parent)
{

}

WebChapter::WebChapter(QObject *parent, QString url) :
    Chapter(parent)
{
    //this->parentWindow = *ui;
    this->linkList.clear();
    connect(&chapterManager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
    connect(&parseManager, SIGNAL(finished(QNetworkReply*)),
            SLOT(initClass(QNetworkReply*)));
    connect(this, SIGNAL(initFinished(WebChapter*)),
                         this->parent(), SLOT(getChaptersToList(WebChapter*)));
    connect(this, SIGNAL(setDownloadProgress(int)),
                         this->parent(), SLOT(setDownloadProgress(int)));
    connect(this, SIGNAL(setDownloadStatus(QString, int, int)),
                         this->parent(), SLOT(setDownloadStatus(QString, int, int)));
    connect(this, SIGNAL(addToDownloadLog(QUrl)),
                         this->parent(), SLOT(addToDownloadLog(QUrl)));
    connect(this, SIGNAL(finishDownloadChapter(WebChapter*)),
                         this->parent(), SLOT(finishDownloadChapter(WebChapter*)));
    volume = 0;
    chapter = 0;
    manga = "";
    this->chapterLink = url + "?mature=1";
    QNetworkRequest r;
    r.setUrl(QUrl(url + "?mature=1"));
    QNetworkReply *reply = parseManager.get(r);
}

void WebChapter::initClass(QNetworkReply *reply)
{

    // Парсим URL, достаём оттуда название манги, номер тома и главы
    QRegExp url = QRegExp("http://([a-z]+)\.([a-z]+)/(.*)/vol(\[0-9\]+)/(\[0-9\]+)");
    url.indexIn(reply->url().toString());
    manga = url.cap(3);
    volume = url.cap(4).toInt();
    chapter = url.cap(5).toInt();
    // Парсим главы
    QRegExp r = QRegExp("var pictures = \\[\\{(.*)\\}\\]");
    QString data;
    data = reply->readAll();
    r.indexIn(data);
    QString chapters = r.cap(1);
    QStringList chaptersList = chapters.split("},{");
    QRegExp selectUrl("url\:\"(.*)\"\,*");
    foreach(QString chapter, chaptersList)
    {
        selectUrl.indexIn(chapter);
        QUrl chapterUrl(selectUrl.cap(1));
        this->linkList.append(chapterUrl);
    }
    this->initFinished(this);
    return;
}

void WebChapter::downloadChapters()
{
    if (linkList.length() > 0)
    {
        percent = (int) 100 / this->linkList.length();
        QDir().mkdir(this->manga);
        this->tempFolder = this->manga + QDir::separator() + "vol" + QString::number(this->volume) + "ch" + QString::number(this->chapter);
        QDir().mkdir(this->tempFolder);
        downloadedPercent = 0;
        this->setDownloadProgress(downloadedPercent);
        this->setDownloadStatus(this->manga, this->volume, this->chapter);
        foreach(QUrl url, this->linkList)
        {
            this->downloadFile(url);
        }
    }
}

void WebChapter::downloadFile(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply *reply = chapterManager.get(request);
    currentDownloads.append(reply);
}

void WebChapter::downloadFinished(QNetworkReply *reply)
{
    downloadedPercent += percent;
    this->setDownloadProgress(downloadedPercent);
    this->setDownloadStatus(this->manga, this->volume, this->chapter);
    QUrl url = reply->url();
    this->addToDownloadLog(url);
    if (reply->error())  {
        this->downloadFile(reply->url());
    } else  {
        QString path = url.path();
        QString filename = QFileInfo(path).fileName();
        QFile file(this->tempFolder + QDir::separator() +filename);
        file.open(QIODevice::WriteOnly);
        file.write(reply->readAll());
        file.close();
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty())
    {
        // Завершаем скачивание
        downloadedPercent = 100;
        this->setDownloadProgress(downloadedPercent);
        this->finishDownloadChapter(this);
    }

}

LocalChapter::LocalChapter(QObject *parent, QString dir)
{
    QRegExp findVolumeChapter("./(.*)/vol([0-9]+)ch([0-9]+)");
    findVolumeChapter.indexIn(dir);
    this->manga = findVolumeChapter.cap(1);
    this->volume = (int) findVolumeChapter.cap(2).toInt();
    this->chapter = (int) findVolumeChapter.cap(3).toInt();
}
