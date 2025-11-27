#include <gtest/gtest.h>
#include <arpa/inet.h>

#include <iostream>
#include <thread>

#include <unistd.h>

#include "common/logger.h"
#include "common/nexthopgroupfull.h"

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
    vector<nh_grp_full> test_nh_grp_full_list = {
        make_nh_grp_full(200, 1, 0),
        make_nh_grp_full(300, 1, 2),
        make_nh_grp_full(310, 2, 0),
        make_nh_grp_full(320, 2, 0),
        make_nh_grp_full(400, 1, 0)
    };
    vector<uint32_t> test_depends = {200, 300, 400};
    vector<uint32_t> test_dependents = {500, 600};

    /* Call constructor function */
    cout << "[DEBUG] Calling NextHopGroupFull Constructor ..." << endl;
    NextHopGroupFull nhg(test_id, test_key, test_nh_grp_full_list, test_depends, test_dependents);

    /* Check the value of the constructed NextHopGroupFull */
    cout << "[DEBUG] Checking constructed values ..." << endl;
    // Check plain values
    EXPECT_EQ(nhg.id, test_id);
    EXPECT_EQ(nhg.key, test_key);
    // Check nh_grp_full_list status
    EXPECT_EQ(nhg.nh_grp_full_list.size(), test_nh_grp_full_list.size());
    for (size_t i = 0; i < test_nh_grp_full_list.size(); i++) {
        EXPECT_EQ(nhg.nh_grp_full_list[i].id, test_nh_grp_full_list[i].id);
        EXPECT_EQ(nhg.nh_grp_full_list[i].weight, test_nh_grp_full_list[i].weight);
        EXPECT_EQ(nhg.nh_grp_full_list[i].num_direct, test_nh_grp_full_list[i].num_direct);
    }
    // Check depends status
    EXPECT_EQ(nhg.depends.size(), test_depends.size());
    for (size_t i = 0; i < test_depends.size(); i++) {
        EXPECT_EQ(nhg.depends[i], test_depends[i]);
    }
    // Check dependents status
    EXPECT_EQ(nhg.dependents.size(), test_dependents.size());
    for (size_t i = 0; i < test_dependents.size(); i++) {
        EXPECT_EQ(nhg.dependents[i], test_dependents[i]);
    }
    // Check other unused values
    cout << "[DEBUG] Checking default values ..." << endl;
    EXPECT_EQ(nhg.weight, 0);
    EXPECT_EQ(nhg.flags, 0);
    EXPECT_TRUE(nhg.ifname.empty());
    EXPECT_EQ(nhg.type, NEXTHOP_TYPE_INVALID);
    EXPECT_EQ(nhg.vrf_id, 0);
    EXPECT_EQ(nhg.ifindex, 0);
    EXPECT_EQ(nhg.nh_label_type, ZEBRA_LSP_NONE);

    union g_addr zero_g_addr = {};
    EXPECT_TRUE(memcmp(&nhg.gate, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_EQ(nhg.bh_type, BLACKHOLE_UNSPEC);
    EXPECT_TRUE(memcmp(&nhg.src, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&nhg.rmap_src, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_EQ(nhg.nh_srv6, nullptr);

    cout << "TEST_NextHopGroupFull::multi_nexthop finished." << endl;

    /* Add test for operator = in multi_nexthop case */
    cout << "TEST_NextHopGroupFull::operator =  for multi_nexthop started:" << endl;
    cout << "[DEBUG] Declare a NextHopGroupFull then assign it with the constructed nhg ..." << endl;
    NextHopGroupFull assigned_nhg;
    assigned_nhg = nhg;

    /* Check the value of the assigned NextHopGroupFull */
    cout << "[DEBUG] Checking assigned constructed values ..." << endl;
    // Check plain values
    EXPECT_EQ(assigned_nhg.id, nhg.id);
    EXPECT_EQ(assigned_nhg.key, nhg.key);
    // Check nh_grp_full_list status
    EXPECT_EQ(assigned_nhg.nh_grp_full_list.size(), nhg.nh_grp_full_list.size());
    for (size_t i = 0; i < nhg.nh_grp_full_list.size(); i++) {
        EXPECT_EQ(assigned_nhg.nh_grp_full_list[i].id, nhg.nh_grp_full_list[i].id);
        EXPECT_EQ(assigned_nhg.nh_grp_full_list[i].weight, nhg.nh_grp_full_list[i].weight);
        EXPECT_EQ(assigned_nhg.nh_grp_full_list[i].num_direct, assigned_nhg.nh_grp_full_list[i].num_direct);
    }
    // Check depends status
    EXPECT_EQ(assigned_nhg.depends.size(), nhg.depends.size());
    for (size_t i = 0; i < nhg.depends.size(); i++) {
        EXPECT_EQ(assigned_nhg.depends[i], nhg.depends[i]);
    }
    // Check dependents status
    EXPECT_EQ(assigned_nhg.dependents.size(), nhg.dependents.size());
    for (size_t i = 0; i < nhg.dependents.size(); i++) {
        EXPECT_EQ(assigned_nhg.dependents[i], nhg.dependents[i]);
    }
    // Check other unused values
    cout << "[DEBUG] Checking assigned default values ..." << endl;
    EXPECT_EQ(assigned_nhg.weight, nhg.weight);
    EXPECT_EQ(assigned_nhg.flags, nhg.flags);
    EXPECT_TRUE(assigned_nhg.ifname.empty());
    EXPECT_EQ(assigned_nhg.type, NEXTHOP_TYPE_INVALID);
    EXPECT_EQ(assigned_nhg.vrf_id, 0);
    EXPECT_EQ(assigned_nhg.ifindex, 0);
    EXPECT_EQ(assigned_nhg.nh_label_type, ZEBRA_LSP_NONE);

    EXPECT_TRUE(memcmp(&assigned_nhg.gate, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_EQ(assigned_nhg.bh_type, BLACKHOLE_UNSPEC);
    EXPECT_TRUE(memcmp(&assigned_nhg.src, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&assigned_nhg.rmap_src, &zero_g_addr, sizeof(g_addr)) == 0);
    EXPECT_EQ(assigned_nhg.nh_srv6, nullptr);

    cout << "TEST_NextHopGroupFull::operator = for multi_nexthop finished." << endl;

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
    EXPECT_EQ(nhg.nh_srv6->seg6local_action, test_nh_srv6.seg6local_action);
    // Check nh_srv6's seg6local_context
    EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6local_ctx.nh6, &test_seg6local_ctx.nh6, sizeof(in6_addr)) == 0);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.table, test_nh_srv6.seg6local_ctx.table);
    // Check nh_srv6's seg6local_ctx's flv
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.flv_ops, test_nh_srv6.seg6local_ctx.flv.flv_ops);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.lcblock_len, test_nh_srv6.seg6local_ctx.flv.lcblock_len);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.flv.lcnode_func_len, test_nh_srv6.seg6local_ctx.flv.lcnode_func_len);
    // flv end
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.block_len, test_nh_srv6.seg6local_ctx.block_len);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.node_len, test_nh_srv6.seg6local_ctx.node_len);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.function_len, test_nh_srv6.seg6local_ctx.function_len);
    EXPECT_EQ(nhg.nh_srv6->seg6local_ctx.argument_len, test_nh_srv6.seg6local_ctx.argument_len);
    // Check nh_srv6's seg6_seg_stack
    EXPECT_NE(nhg.nh_srv6->seg6_segs, nullptr);
    EXPECT_EQ(nhg.nh_srv6->seg6_segs->encap_behavior, test_nh_seg6_segs.encap_behavior);
    EXPECT_EQ(nhg.nh_srv6->seg6_segs->num_segs, test_nh_seg6_segs.num_segs);
    // Check nh_srv6's seg6_seg_stack's seg list
    for (size_t i = 0; i < test_nh_seg6_segs.num_segs; i++) {
        if (i < test_nh_segs.size()) {
            EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6_segs->seg[i], &test_nh_segs[i], sizeof(in6_addr)) == 0)
                << "Mismatch in segment " << i << endl;
        } else {
            in6_addr zero = {};
            memset(&zero, 0, sizeof(zero));
            EXPECT_TRUE(memcmp(&nhg.nh_srv6->seg6_segs->seg[i], &zero, sizeof(in6_addr)) == 0);
        }
    }
    // Check other unused values
    EXPECT_TRUE(nhg.nh_grp_full_list.empty());
    EXPECT_TRUE(nhg.depends.empty());
    EXPECT_TRUE(nhg.dependents.empty());

    cout << "TEST_NextHopGroupFull::singleton finished." << endl;

    /* Add test for operator = in singleton case */
    cout << "TEST_NextHopGroupFull::operator = for singleton started:" << endl;
    cout << "[DEBUG] Declare a NextHopGroupFull then assign it with the constructed nhg ..." << endl;
    NextHopGroupFull assigned_nhg;
    assigned_nhg = nhg;

    /* Check the value of the assigned NextHopGroupFull */
    cout << "[DEBUG] Checking assigned constructed values ..." << endl;
    // Check plain values
    EXPECT_EQ(assigned_nhg.id, nhg.id);
    EXPECT_EQ(assigned_nhg.key, nhg.key);
    EXPECT_EQ(assigned_nhg.type, nhg.type);
    EXPECT_EQ(assigned_nhg.vrf_id, nhg.vrf_id);
    EXPECT_EQ(assigned_nhg.ifindex, nhg.ifindex);
    EXPECT_EQ(assigned_nhg.ifname, nhg.ifname);
    EXPECT_EQ(assigned_nhg.nh_label_type, nhg.nh_label_type);
    EXPECT_EQ(assigned_nhg.flags, nhg.flags);

    // Check address values
    EXPECT_TRUE(memcmp(&assigned_nhg.gate, &nhg.gate, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&assigned_nhg.src, &nhg.src, sizeof(g_addr)) == 0);
    EXPECT_TRUE(memcmp(&assigned_nhg.rmap_src, &nhg.rmap_src, sizeof(g_addr)) == 0);

    // Check SRv6 info
    EXPECT_NE(assigned_nhg.nh_srv6, nullptr);
    // Check nh_srv6's seg6local_action
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_action, nhg.nh_srv6->seg6local_action);
    // Check nh_srv6's seg6local_context
    EXPECT_TRUE(memcmp(&assigned_nhg.nh_srv6->seg6local_ctx.nh6,
                    &nhg.nh_srv6->seg6local_ctx.nh6, sizeof(in6_addr)) == 0);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.table, nhg.nh_srv6->seg6local_ctx.table);
    // Check nh_srv6's seg6local_ctx's flv
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.flv.flv_ops, nhg.nh_srv6->seg6local_ctx.flv.flv_ops);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.flv.lcblock_len,
            nhg.nh_srv6->seg6local_ctx.flv.lcblock_len);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.flv.lcnode_func_len,
            nhg.nh_srv6->seg6local_ctx.flv.lcnode_func_len);
    // flv end
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.block_len, nhg.nh_srv6->seg6local_ctx.block_len);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.node_len, nhg.nh_srv6->seg6local_ctx.node_len);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.function_len, nhg.nh_srv6->seg6local_ctx.function_len);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6local_ctx.argument_len, nhg.nh_srv6->seg6local_ctx.argument_len);
    // Check nh_srv6's seg6_seg_stack
    EXPECT_NE(assigned_nhg.nh_srv6->seg6_segs, nullptr);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6_segs->encap_behavior, nhg.nh_srv6->seg6_segs->encap_behavior);
    EXPECT_EQ(assigned_nhg.nh_srv6->seg6_segs->num_segs, nhg.nh_srv6->seg6_segs->num_segs);
    // Check nh_srv6's seg6_seg_stack's seg list
    for (size_t i = 0; i < nhg.nh_srv6->seg6_segs->num_segs; i++) {
        if (i < nhg.nh_srv6->seg6_segs->num_segs) {
            EXPECT_TRUE(memcmp(&assigned_nhg.nh_srv6->seg6_segs->seg[i],
                            &nhg.nh_srv6->seg6_segs->seg[i], sizeof(in6_addr)) == 0)
                << "Mismatch in segment " << i << endl;
        } else {
            in6_addr zero = {};
            memset(&zero, 0, sizeof(zero));
            EXPECT_TRUE(memcmp(&assigned_nhg.nh_srv6->seg6_segs->seg[i], &zero, sizeof(in6_addr)) == 0);
        }
    }
    // Check other unused values
    EXPECT_TRUE(assigned_nhg.nh_grp_full_list.empty());
    EXPECT_TRUE(assigned_nhg.depends.empty());
    EXPECT_TRUE(assigned_nhg.dependents.empty());

    cout << "TEST_NextHopGroupFull::operator = for singleton finished." << endl;
}