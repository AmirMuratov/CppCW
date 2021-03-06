#-------------------------------------------------
#
# Project created by QtCreator 2015-08-15T12:23:10
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = tictactoe
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11
CONFIG += address_sanitizer

TEMPLATE = app


SOURCES += main.cpp \
    library/HttpServer.cpp \
    library/TcpServer.cpp \
    library/TcpSocket.cpp \
    library/HttpRequest.cpp \
    library/HttpSocket.cpp \
    library/HttpResponse.cpp \
    Game.cpp \
    TicTacToe.cpp \
    library/EpollWrap.cpp

HEADERS += \
    library/HttpServer.h \
    library/TcpServer.h \
    library/TcpSocket.h \
    library/HttpRequest.h \
    library/HttpSocket.h \
    library/HttpResponse.h \
    Game.h \
    TicTacToe.h \
    library/EpollWrap.h

RESOURCES += \
    res/Resources.qrc
