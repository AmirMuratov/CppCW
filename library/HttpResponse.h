#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QString>
#include <iostream>

class HttpResponse {
public:
    QString header;
    const char * content;
    size_t content_length, headers_length;
    HttpResponse(const int status, const char * server_name, const char * content_type, const size_t content_length, const char * content);
    int getContentSize() const;
    int getHeadersSize() const;
    const char * getHeaders() const;
    const char * getContent() const;
};

#endif // HTTPRESPONSE_H
