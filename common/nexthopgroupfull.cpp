#include "nexthopgroupfull.h"
#include "common/logger.h"

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <cstdint>

using namespace std;
using namespace swss;

/* Constructor for multi-path NextHopGroupFull */
NextHopGroupFull::NextHopGroupFull(std::uint32_t id_in, std::uint32_t key_in,
                const std::vector<nh_grp_full>& nh_grp_full_list_in,
                const std::vector<uint32_t>& depends_in,
                const std::vector<uint32_t>& dependents_in)
    : id(id_in), key(key_in), nh_grp_full_list(nh_grp_full_list_in), depends(depends_in), dependents(dependents_in)
{
    SWSS_LOG_DEBUG("NextHopGroupFull construction started (multi-nexthop)");
    cout << "[CPP DEBUG] NextHopGroupFull construction started (multi-nexthop)" << endl;
    memset(&gate, 0, sizeof(gate));
    bh_type = BLACKHOLE_UNSPEC;
    memset(&src, 0, sizeof(src));
    memset(&rmap_src, 0, sizeof(rmap_src));
    SWSS_LOG_DEBUG("NextHopGroupFull construction finished (multi-nexthop)");
    cout << "[CPP DEBUG] NextHopGroupFull construction finished (multi-nexthop)" << endl;
}

/* Constructor for singleton NextHopGroupFull */
NextHopGroupFull::NextHopGroupFull(std::uint32_t id_in, std::uint32_t key_in,
                enum nexthop_types_t type_in, vrf_id_t vrf_id_in, ifindex_t ifindex_in,
                std::string ifname_in, enum lsp_types_t label_type_in, enum blackhole_type bh_type_in,
                union g_addr gateway_in, union g_addr src_in, union g_addr rmap_src_in,
                std::uint8_t weight_in, std::uint8_t flags_in, bool has_srv6, bool has_seg6_segs,
                const struct nexthop_srv6* nh_srv6_in,
                const struct seg6_seg_stack* nh_seg6_segs_in,
                const std::vector<struct in6_addr>& nh_segs_in)
    : id(id_in), key(key_in), weight(weight_in), flags(flags_in), ifname(ifname_in),
      type(type_in), vrf_id(vrf_id_in), ifindex(ifindex_in), nh_label_type(label_type_in),
      src(src_in), rmap_src(rmap_src_in), nh_srv6(nullptr)
{
    SWSS_LOG_DEBUG("NextHopGroupFull construction started (singleton)");
    cout << "[CPP DEBUG] NextHopGroupFull construction started (singleton)" << endl;

    /* Initialize union member */
    this->bh_type = bh_type_in;
    this->gate = gateway_in;

    /* Check if need to allocate the nexthop_srv6 structure */
    if (has_srv6 && nh_srv6_in != nullptr)
    {
        SWSS_LOG_DEBUG("NextHopGroupFull has srv6, allocating...");
        cout << "[CPP DEBUG] NextHopGroupFull has srv6, allocating..." << endl;
        nh_srv6 = (struct nexthop_srv6 *)malloc(sizeof(struct nexthop_srv6));
        if (!nh_srv6) {
            SWSS_LOG_ERROR("nh_srv6 allocation failed in NextHopGroupFull constructor, abort");
            return;
        }

        memcpy(nh_srv6, nh_srv6_in, sizeof(struct nexthop_srv6));
        SWSS_LOG_DEBUG("NextHopGroupFull constructor finished nh_srv6 initialization");
        cout << "[CPP DEBUG] NextHopGroupFull constructor finished nh_srv6 initialization" << endl;
    }
    else
        SWSS_LOG_DEBUG("NextHopGroupFull does not have srv6 info");

    /* Check if need to allocate the seg6_seg_stack structure */
    if (has_seg6_segs && nh_seg6_segs_in != nullptr)
    {
        SWSS_LOG_DEBUG("NextHopGroupFull has seg6_segs, allocating...");
        cout << "[CPP DEBUG] NextHopGroupFull has seg6_segs, allocating..." << endl;
        size_t total_size = sizeof(struct seg6_seg_stack) +
                                    nh_seg6_segs_in->num_segs * sizeof(struct in6_addr);
        nh_srv6->seg6_segs = (struct seg6_seg_stack *)malloc(total_size);
        if (!nh_srv6->seg6_segs) {
            SWSS_LOG_ERROR("seg6_segs allocation failed in NextHopGroupFull constructor, abort");
            free(nh_srv6);      /* If fail to allocate segs, we free the nh_srv6 as well */
            nh_srv6 = nullptr;
            return;
        }

        memcpy(nh_srv6->seg6_segs, nh_seg6_segs_in, sizeof(struct seg6_seg_stack));

        if (nh_srv6->seg6_segs->num_segs > 0)
        {
            for (size_t i = 0; i < nh_srv6->seg6_segs->num_segs; i++)
            {
                if (i < nh_segs_in.size())
                    memcpy(&nh_srv6->seg6_segs->seg[i], &nh_segs_in[i], sizeof(struct in6_addr));
                else
                {
                    /* If the number between num and segs are not matching */
                    memset(&nh_srv6->seg6_segs->seg[i], 0, sizeof(in6_addr));
                    SWSS_LOG_DEBUG("Size between num_segs and vector segs is not matching, num_segs %d, vector size %zu",
                                nh_srv6->seg6_segs->num_segs, nh_segs_in.size());
                    cout << "[CPP DEBUG] Size between num_segs and vector segs is not matching, num_segs " << nh_srv6->seg6_segs->num_segs
                        << ", vector size " << nh_segs_in.size() << endl;
                }
            }
        }
    }
    SWSS_LOG_DEBUG("NextHopGroupFull construction finished (singleton)");
    cout << "[CPP DEBUG] NextHopGroupFull construction finished (singleton)" << endl;
}

/* Copy Assignment Operator */
NextHopGroupFull& NextHopGroupFull::operator = (const NextHopGroupFull &other)
{
    if (this == &other)     return *this;

    id = other.id;
    key = other.key;
    weight = other.weight;
    flags = other.flags;

    ifname = other.ifname;

    nh_grp_full_list = other.nh_grp_full_list;
    depends = other.depends;
    dependents = other.dependents;

    type = other.type;
    vrf_id = other.vrf_id;
    ifindex = other.ifindex;
    nh_label_type = other.nh_label_type;
    bh_type = other.bh_type;

    memcpy(&gate, &other.gate, sizeof(g_addr));
    memcpy(&src, &other.src, sizeof(g_addr));
    memcpy(&rmap_src, &other.rmap_src, sizeof(g_addr));

    /* Deep copy for nh_srv6 information */
    if (nh_srv6 != nullptr) {
        delete nh_srv6;
    }
    if (other.nh_srv6 != nullptr) {
        nh_srv6 = new nexthop_srv6();

        nh_srv6->seg6local_action = other.nh_srv6->seg6local_action;
        nh_srv6->seg6local_ctx = other.nh_srv6->seg6local_ctx;

        /* Deep copy the flexible array in seg6_segs */
        if (other.nh_srv6->seg6_segs != nullptr) {
            size_t size = sizeof(seg6_seg_stack)
                            + other.nh_srv6->seg6_segs->num_segs * sizeof(in6_addr);
            nh_srv6->seg6_segs = static_cast<seg6_seg_stack*>(malloc(size));
            memcpy(nh_srv6->seg6_segs, other.nh_srv6->seg6_segs, size);
        } else {
            nh_srv6->seg6_segs = nullptr;
        }
    } else {
        nh_srv6 = nullptr;
    }

    return *this;
}

/* Destructor of NextHopGroupFull */
NextHopGroupFull::~NextHopGroupFull()
{
    SWSS_LOG_DEBUG("NextHopGroupFull destructor started");
    cout << "[CPP DEBUG] NextHopGroupFull destructor started" << endl;
    if (nh_srv6 != nullptr)
    {
        if (nh_srv6->seg6_segs != nullptr)
        {
            SWSS_LOG_DEBUG("Free seg6_segs in NextHopGroupFull Destructor");
            cout << "[CPP DEBUG] Free seg6_segs in NextHopGroupFull Destructor" << endl;
            free(nh_srv6->seg6_segs);
        }
        SWSS_LOG_DEBUG("Free nh_srv6 in NextHopGroupFull Destructor");
        cout << "[CPP DEBUG] Free nh_srv6 in NextHopGroupFull Destructor" << endl;
        free(nh_srv6);
    }

    SWSS_LOG_DEBUG("NextHopGroupFull has been destroyed successfully.");
    cout << "[CPP DEBUG] NextHopGroupFull has been destroyed successfully." << endl;
}