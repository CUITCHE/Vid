#ifndef REQUESTWRAPPER_H
#define REQUESTWRAPPER_H

#include <string>

namespace communication {
    class Request;
};


class RequestWrapper
{
public:
    RequestWrapper(const communication::Request &request);

    int32_t get_code();
    int32_t get_id();
    const std::string& get_body();
private:
    const communication::Request &request;
};

#endif // REQUESTWRAPPER_H
