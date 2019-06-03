#include "RequestWrapper.h"
#include "model/requests.pb.h"
#include "model/base.pb.h"

RequestWrapper::RequestWrapper(const communication::Request &request)
    :request(request)
{

}


int32_t RequestWrapper::get_code() { return request.proto(); }
int32_t RequestWrapper::get_id() { return request.id(); }
const std::string& RequestWrapper::get_body() { return request.body(); }
