#ifndef CHAPTER_H
#define CHAPTER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QDir>
#include <QRegExp>

class Chapter : public QObject
{
private:
    Q_OBJECT

public:
    explicit Chapter(QObject *parent = 0);
    QString manga;
    int chapter, volume;

signals:

public slots:

};

class WebChapter : public Chapter
{
private:
    Q_OBJECT
    QNetworkAccessManager chapterManager;
    QNetworkAccessManager parseManager;
    QList<QNetworkReply *> currentDownloads;
    int percent;
    qint16 downloadedPercent;
public:
    QUrl chapterLink;
    QList<QUrl> linkList;
    explicit WebChapter(QObject *parent = 0, QString url = "");
    void downloadChapters();
    void downloadFile(const QUrl &url);
    QString tempFolder;
    bool operator< (const WebChapter &c1) const
    {
        return c1.volume > volume ? false : (c1.volume < volume ? true : (c1.chapter > chapter ? false : (c1.chapter < chapter ? true : false)));
    }
    bool operator> (const WebChapter &c1) const
    {
        return c1.volume < volume ? false : (c1.volume > volume ? true : (c1.chapter < chapter ? false : (c1.chapter > chapter ? true : false)));
    }

signals:
    void initFinished(WebChapter* chapter);
    void setDownloadProgress(int percent);
    void setDownloadStatus(QString manga, int volume, int chapter);
    void addToDownloadLog(QUrl url);
    void finishDownloadChapter(WebChapter *chapter);

public slots:
    void downloadFinished(QNetworkReply* reply);
    void initClass(QNetworkReply* reply);

};

class LocalChapter: public Chapter
{
private:
    Q_OBJECT
public:
    QDir chapterDirectory;
    explicit LocalChapter(QObject *parent = 0, QString dir = "");
};

#endif // CHAPTER_H
