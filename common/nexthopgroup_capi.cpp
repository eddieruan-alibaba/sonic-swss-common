// nexthopgroup_capi.cpp

#include "common/nexthopgroupfull.h"
#include "common/nexthopgroupfull_json.h"
#include "common/nexthopgroup_capi.h"
#include <swss/ipaddress.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>


extern "C" {

NextHopGroupFull* nexthopgroup_create(void)
{
    try {
        return new NextHopGroupFull();
    } catch (...) {
        return nullptr;
    }
}

void nexthopgroup_free(NextHopGroupFull* obj)
{
    delete obj;
}

char* nexthopgroup_to_json(const NextHopGroupFull* obj)
{
    if (!obj) {
        return nullptr;
    }

    try {
        std::string json_str = to_json_string(*obj);
        char* c_str = static_cast<char*>(std::malloc(json_str.size() + 1));
        if (c_str) {
            std::memcpy(c_str, json_str.c_str(), json_str.size() + 1);
        }
        return c_str;
    } catch (...) {
        return nullptr;
    }
}

int nexthopgroup_from_json(const char* json_str, swss::NextHopGroupFull** out_obj)
{
    if (!json_str || !out_obj) return -1;
    try {
        swss::NextHopGroupFull* obj = new swss::NextHopGroupFull();
        if (swss::from_json_string(json_str, *obj)) {
            *out_obj = obj;
            return 0;
        } else {
            delete obj;
            return -1;
        }
    } catch (...) {
        return -1;
    }
}

} // extern "C"