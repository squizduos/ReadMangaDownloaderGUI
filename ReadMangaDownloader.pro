#-------------------------------------------------
#
# Project created by QtCreator 2014-09-01T18:24:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReadMangaDownloader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    manga.cpp \
    chapter.cpp

HEADERS  += mainwindow.h \
    manga.h \
    chapter.h

FORMS    += mainwindow.ui
