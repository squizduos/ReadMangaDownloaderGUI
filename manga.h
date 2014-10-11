#ifndef MANGA_H
#define MANGA_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>
#include <QDir>
#include <chapter.h>

class Manga : public QObject
{
    Q_OBJECT
    private:
        QNetworkAccessManager parseManager;
public:
    explicit Manga(QObject *parent = 0, QString url = "");
    template<class T> void appendChapter(const T &chapter);
    QList<WebChapter *> webChapters;
    QString name;
    QString author;
    QString url;
    QString site;


signals:
   void initFinished();
   void setChaptersCount();
   void errorLoadingManga(QString);

public slots:
    void initClass(QNetworkReply *reply);
};

#endif // MANGA_H
