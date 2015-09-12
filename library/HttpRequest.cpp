#include "HttpRequest.h"

HttpRequest::HttpRequest() {
    valid = 0;
}

HttpRequest::HttpRequest(const QString& qdata) {
    if (qdata.size() == 0) {
        return;
    }
    std::vector<QStringList> tokens;
    int l = 0, r;
    for (r = qdata.indexOf("\r\n"); r != -1; r = qdata.indexOf("\r\n", l)) {
        tokens.push_back(qdata.mid(l, r - l).split(' '));
        if (l == r) {
            l = r + 2;
            break;
        }
        l = r + 2;
    }
    data = qdata.mid(l);
    valid = 1;
    if (tokens[0].size() != 3) {
        valid = 0;
        return;
    }
    if (tokens.size() < 2) {
        valid = 0;
        return;
    }
    method = tokens[0].at(0);
    uri = tokens[0].at(1);
    version = tokens[0].at(2);
    size_t i;
    for (i = 1; i < tokens.size() && tokens[i].size() != 0 && tokens[i].at(0).length() != 0; i++) {        
        headers.push_back(tokens[i]);
    }
}

bool HttpRequest::isValid() const{
    return valid;
}

void HttpRequest::clear() {
    valid = 0;
    method.clear();
    version.clear();
    uri.clear();
    headers.clear();
    data.clear();
}

const std::string& HttpRequest::toString() const {
    std::string s = method.toStdString() + " " + uri.toStdString() + " " + version.toStdString() + "\r\n";
    for (size_t i = 0; i < headers.size(); i++) {
        for (int j = 0; j < headers[i].size(); j++) {
            s += headers[i].at(j).toStdString();
            if (j < headers[i].size() - 1) {
                s += " ";
            }
        }
        s += "\r\n";
    }
    s += "\r\n";
    s += data.toStdString();
    return s;
}
