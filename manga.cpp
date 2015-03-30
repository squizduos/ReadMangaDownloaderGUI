#include "manga.h"

Manga::Manga(QObject *parent, QString url) :
    QObject(parent)
{
    connect(&parseManager, SIGNAL(finished(QNetworkReply*)),
            SLOT(initClass(QNetworkReply*)));
    connect(this, SIGNAL(initFinished()),
            this->parent(), SLOT(updateManga()));
    connect(this, SIGNAL(setChaptersCount()),
            this->parent(), SLOT(setMangaChaptersCount()));
    connect(this, SIGNAL(errorLoadingManga(QString)),
            this->parent(), SLOT(errorLoadingManga(QString)));
    // Инициализируем список
    webChapters.clear();
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply *reply = parseManager.get(request);
}

void Manga::initClass(QNetworkReply *reply)
{
    if (reply->error())
    {
        this->errorLoadingManga(reply->errorString());
        return;
    }
    QString data = reply->readAll();
    // Парсим название манги
    QRegExp findName = QRegExp("<meta itemprop=\"name\" content=\"(.*)\" />...<meta itemprop=\"alternativeHeadline\"");
    findName.indexIn(data);
    this->name = findName.cap(1);
    // Парсим автора манги
    QRegExp findAuthor = QRegExp("<span class=\"elem_author \"> <a href=\"/list/person/([a-z, _]+)\" class=\"person-link\">([a-z, A-Z, 0-9, А-Я, а-я]+)</a><a class='screenshot'");
    findAuthor.indexIn(data);
    this->author = findAuthor.cap(2);
    // Парсим nickname манги и сайт с мангой
    QRegExp findUrl = QRegExp("http://([a-z, \.]+)/(.*)");
    findUrl.indexIn(reply->url().toString());
    url = findUrl.cap(2);
    site = findUrl.cap(1);
    // Парсим главы манги
    QRegExp findChapters("<table class=\"table table-hover\" id=\"chapters-list\">(.*)</table>");
    findChapters.indexIn(data);
    QString chaptersString = findChapters.cap(1);
    QStringList chaptersList = chaptersString.split("</tr>");
    QRegExp findChapterUrl = QRegExp("<a href=\"([a-z, 0-9, _, //]+)\" title=\"\">");
    // Считаем количество глав
    foreach(QString chapterString, chaptersList)
    {
        findChapterUrl.indexIn(chapterString);
        if (findChapterUrl.cap(1).length() > 0)
        {
            this->setChaptersCount();
        }
    }

    foreach(QString chapterString, chaptersList)
    {
        findChapterUrl.indexIn(chapterString);
        if (findChapterUrl.cap(1).length() > 0)
        {
            WebChapter* ch = new WebChapter(this->parent(), QString("http://" + site + findChapterUrl.cap(1)));
        }
    }
    this->initFinished();
}

