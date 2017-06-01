#include "ExceptionWrapper.h"

#include <regex>

using namespace std;

ExceptionWrapper::ExceptionWrapper(const std::runtime_error err, const char * file, int line) : std::runtime_error(err) {
    ostringstream o;
    regex r(".*[\\\\]([^\\\\]*)");
    smatch m;
    string s(file);
    std::regex_search(s, m, r);
    o << m[1] << ":" << line << ":" << err.what();
    msg = o.str();
}

ExceptionWrapper::~ExceptionWrapper() throw() {}

const char * ExceptionWrapper::what() const throw() {
    return msg.c_str();
}
