#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <QStringList>
#include <QTextStream>
#include <iostream>

class HttpRequest{
public:
    bool valid;
    QString method;
    QString version;
    QString uri;
    std::vector<QStringList> headers;
    QString data;

    HttpRequest();
    HttpRequest(const QString&);
    bool isValid() const;
    void clear();
    const std::string& toString() const;
};

#endif // HTTPREQUEST_H
