#-------------------------------------------------
#
# Project created by QtCreator 2015-01-15T19:57:15
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
    lib/http/HttpServer.cpp \
    lib/tcp/TcpServer.cpp \
    lib/tcp/TcpSocket.cpp \
    lib/http/HttpRequest.cpp \
    lib/http/HttpSocket.cpp \
    lib/http/HttpResponse.cpp \
    Game.cpp \
    TicTacToe.cpp

HEADERS += \
    lib/http/HttpServer.h \
    lib/tcp/TcpServer.h \
    lib/tcp/TcpSocket.h \
    lib/http/HttpRequest.h \
    lib/http/HttpSocket.h \
    lib/http/HttpResponse.h \
    Game.h \
    TicTacToe.h

RESOURCES += \
    res/Resources.qrc
