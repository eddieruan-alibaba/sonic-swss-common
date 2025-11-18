#include <gtest/gtest.h>
#include <arpa/inet.h>

#include <iostream>
#include <thread>

#include <unistd.h>

#include "common/logger.h"
#include "common/nexthopgroup/nexthopgroupfull.h"
#include "common/nexthopgroup/nexthopgroupfull_json.h"

using namespace std;
using namespace swss;

nh_grp_full make_nh_grp_full(uint32_t id, uint8_t weight, uint32_t num_direct) {
    return {id, weight, num_direct};
}

TEST(NextHopGroupFull, multi_nexthop)
{
    cout << "TEST_NextHopGroupFull::multi_nexthop started: " << endl;
    cout << "[DEBUG] Constructing values ..." << endl;
    /* Prepare the parameters */
    uint32_t test_id = 100;
    uint32_t test_key = 1234567;
    vector<nh_grp_full> test_depends = {
        make_nh_grp_full(200, 1, 0),
        make_nh_grp_full(300, 1, 2),
        make_nh_grp_full(310, 2, 0),
        make_nh_grp_full(320, 2, 0),
        make_nh_grp_full(400, 1, 0)
    };
    vector<nh_grp_full> test_dependents = {
        make_nh_grp_full(500, 1, 2),
        make_nh_grp_full(510, 2, 0),
        make_nh_grp_full(520, 2, 0),
        make_nh_grp_full(600, 1, 0)
    };

    /* Call constructor function */
    cout << "[DEBUG] Calling NextHopGroupFull Constructor ..." << endl;
    NextHopGroupFull nhg(test_id, test_key, test_depends, test_dependents);

    /* Check the value of the constructed NextHopGroupFull */
    cout << "[DEBUG] Checking constructed values ..." << endl;
    // Check plain values
    EXPECT_EQ(nhg.id, test_id);
    EXPECT_EQ(nhg.key, test_key);
    // Check depends status
    EXPECT_EQ(nhg.depends.size(), test_depends.size());
    for (size_t i = 0; i < test_depends.size(); i++) {
        EXPECT_EQ(nhg.depends[i].id, test_depends[i].id);
        EXPECT_EQ(nhg.depends[i].weight, test_depends[i].weight);
        EXPECT_EQ(nhg.depends[i].num_direct, test_depends[i].num_direct);
    }
    // Check dependents status
    EXPECT_EQ(nhg.dependents.size(), test_dependents.size());
    for (size_t i = 0; i < test_dependents.size(); i++) {
        EXPECT_EQ(nhg.dependents[i].id, test_dependents[i].id);
        EXPECT_EQ(nhg.dependents[i].weight, test_dependents[i].weight);
        EXPECT_EQ(nhg.dependents[i].num_direct, test_dependents[i].num_direct);
    }
    // Check other unused values
    cout << "[DEBUG] Checking default values ..." << endl;
    EXPECT_EQ(nhg.weight, 0);
    EXPECT_EQ(nhg.flags, 0);
    EXPECT_TRUE(nhg.ifname.empty());
    EXPECT_EQ(nhg.nh_srv6, nullptr);

    // Test  Serializing API
    string json_str = to_json_string(nhg);
    std::cout << "Serialized NHG to JSON str: " << json_str << std::endl;

    NextHopGroupFull out_obj;
    bool ret = from_json_string(json_str, out_obj);
    std::cout << "Deserialized NHG from JSON str: "  << ret << std::endl;
    EXPECT_EQ(ret, true);

    cout << "TEST_NextHopGroupFull::multi_nexthop finished." << endl;
}


TEST(NextHopGroupFull, singleton)
{
    cout << "TEST_NextHopGroupFull::singleton_no_srv6 started: " << endl;
    cout << "[DEBUG] Constructing values ..." << endl;

    /* Prepare the parameters */
    uint32_t test_id = 100;
    uint32_t test_key = 1234567;
    enum nexthop_types_t test_type = NEXTHOP_TYPE_IPV6;
    vrf_id_t test_vrf_id = 101;
    ifindex_t test_ifindex = 101;
    string test_ifname = "eth101";
    enum lsp_types_t test_nh_label_type = ZEBRA_LSP_NONE;
    enum blackhole_type test_bh_type = BLACKHOLE_NULL;

    union g_addr test_gateway = {};
    union g_addr test_src = {};
    union g_addr test_rmap_src = {};
    inet_pton(AF_INET6, "2001:db8::1", &test_gateway.ipv6.s6_addr);
    inet_pton(AF_INET6, "2001:db8::2", &test_src.ipv6.s6_addr);
    inet_pton(AF_INET6, "2001:db8::3", &test_rmap_src.ipv6.s6_addr);

    uint8_t test_weight = 8;
    uint8_t test_flags = 8;
    bool test_has_srv6 = true;
    bool test_has_seg6_segs = true;

    // Prepare the segment list
    vector<struct in6_addr> test_nh_segs {
        {}, {}, {}
    };
    inet_pton(AF_INET6, "2001:db8:1::1", test_nh_segs[0].s6_addr);
    inet_pton(AF_INET6, "2001:db8:1::2", test_nh_segs[1].s6_addr);
    inet_pton(AF_INET6, "2001:db8:1::3", test_nh_segs[2].s6_addr);

    // Prepare seg6_segs
    struct seg6_seg_stack test_nh_seg6_segs = {};
    test_nh_seg6_segs.encap_behavior = SRV6_HEADEND_BEHAVIOR_H_ENCAPS;
    test_nh_seg6_segs.num_segs = 3;
    memcpy(test_nh_seg6_segs.seg, test_nh_segs.data(), test_nh_segs.size() * sizeof(in6_addr));

    //Prepare seg6local_flavors_info
    struct seg6local_flavors_info test_flv = {
        .flv_ops = 100,
        .lcblock_len = 20,
        .lcnode_func_len = 16
    };

    // Prepare seg6local_ctx
    struct seg6local_context test_seg6local_ctx = {};
    test_seg6local_ctx.table = 100;
    test_seg6local_ctx.block_len = 36;
    test_seg6local_ctx.node_len = 12;
    test_seg6local_ctx.function_len = 20;
    test_seg6local_ctx.argument_len = 16;
    inet_pton(AF_INET6, "2001:db8:2::1", test_seg6local_ctx.nh6.s6_addr);
    memcpy(&test_seg6local_ctx.flv, &test_flv, sizeof(struct seg6local_flavors_info));

    // Prepare nh_srv6
    struct nexthop_srv6 test_nh_srv6 = {};
    test_nh_srv6.seg6local_action = SEG6_LOCAL_ACTION_END_DT6;
    memcpy(&test_nh_srv6.seg6local_ctx, &test_seg6local_ctx, sizeof(struct seg6local_context));

    /* Call constructor function */
    cout << "[DEBUG] Calling NextHopGroupFull Constructor ..." << endl;
    NextHopGroupFull nhg(test_id, test_key, test_type, test_vrf_id, test_ifindex,
                test_ifname, test_nh_label_type, test_bh_type,
                test_gateway, test_src, test_rmap_src,
                test_weight, test_flags, test_has_srv6, test_has_seg6_segs,
                &test_nh_srv6, &test_nh_seg6_segs, test_nh_segs);


    /* Check the value of the constructed NextHopGroupFull */
    cout << "[DEBUG] Checking constructed values ..." << endl;
    // Check plain values
    EXPECT_EQ(nhg.id, test_id);
    EXPECT_EQ(nhg.key, test_key);
    EXPECT_EQ(nhg.type, test_type);
    EXPECT_EQ(nhg.vrf_id, test_vrf_id);
    EXPECT_EQ(nhg.ifindex, test_ifindex);
    EXPECT_EQ(nhg.ifname, test_ifname);
    EXPECT_EQ(nhg.nh_label_type, test_nh_label_type);
    EXPECT_EQ(nhg.weight, test_weight);
    EXPECT_EQ(nhg.flags, test_flags);

    // Check address values
    EXPECT_TRUE(memcmp(&nhg.gate, &test_gateway, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&nhg.src, &test_src, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&nhg.rmap_src, &test_rmap_src, sizeof(g_addr)) == 0);

    // Check SRv6 info
    EXPECT_NE(nhg.nh_srv6, nullptr);
    // Check nh_srv6's seg6local_action
    EXPECT_EQ(nhg.nh_srv6->seg6local_action, SEG6_LOCAL_ACTION_END_DT6);
    // Check nh_srv6's seg6local_context
    EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6local_ctx.nh6, &test_seg6local_ctx.nh6, sizeof(in6_addr)) == 0);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.table, 100);
    // Check nh_srv6's seg6local_ctx's flv
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.flv_ops, 100);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.lcblock_len, 20);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.lcnode_func_len, 16);
    // flv end
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.block_len, 36);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.node_len, 12);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.function_len, 20);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.argument_len, 16);
    // Check nh_srv6's seg6_seg_stack
    EXPECT_NE(nhg.nh_srv6->seg6_segs, nullptr);
    EXPECT_EQ(nhg.nh_srv6->seg6_segs->encap_behavior, SRV6_HEADEND_BEHAVIOR_H_ENCAPS);
    EXPECT_EQ(nhg.nh_srv6->seg6_segs->num_segs, test_nh_segs.size());
    // Check nh_srv6's seg6_seg_stack's seg list
    for (size_t i = 0; i < test_nh_seg6_segs.num_segs; i++) {
        if (i < test_nh_segs.size()) {
            EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6_segs->seg[i], &test_nh_segs[i], sizeof(in6_addr)) == 0)
                << "Mismatch in segment" << i << endl;
        } else {
            in6_addr zero = {};
            memset(&zero, 0, sizeof(zero));
            EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6_segs->seg[i], &zero, sizeof(in6_addr)) == 0);
        }
    }

    cout << "TEST_NextHopGroupFull::singleton finished." << endl;
}