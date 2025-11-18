// nexthopgroup_capi.cpp

#include "common/nexthopgroup/nexthopgroupfull.h"
#include "common/nexthopgroup/nexthopgroupfull_json.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>


// Optional: bring into global scope for convenience
using NextHopGroupFull = swss::NextHopGroupFull;

// Declare C-compatible API directly (no need for header here)
extern "C" {

NextHopGroupFull* nexthopgroup_create(void);
void nexthopgroup_free(NextHopGroupFull* obj);
char* nexthopgroup_to_json(NextHopGroupFull* obj);

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

char* nexthopgroup_to_json(NextHopGroupFull* obj)
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


} // extern "C"