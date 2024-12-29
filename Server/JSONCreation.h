//
// Created by Meverlk on 29.12.2024.
//

#ifndef JSONCREATION_H
#define JSONCREATION_H

std::string escapeSpecialCharacters(const std::string& str) {
    std::string escaped;
    for (char ch : str) {
        if (ch == '"') {
            escaped += "\\\"";
        } else if (ch == '\\') {
            escaped += "\\\\";
        } else if (ch == '\n') {
            escaped += "\\n";
        } else if (ch == '\r') {
            escaped += "\\r";
        } else if (ch == '\t') {
            escaped += "\\t";
        } else {
            escaped += ch;
        }
    }
    return escaped;
}

std::string createJsonResponse(const std::vector<std::string>& results) {
    std::ostringstream json;
    json << "{ \"results\": [";
    for (size_t i = 0; i < results.size(); ++i) {
        json << "\"" << escapeSpecialCharacters(results[i]) << "\"";
        if (i != results.size() - 1) {
            json << ", ";
        }
    }
    json << "] }";
    return json.str();
}

#endif //JSONCREATION_H
