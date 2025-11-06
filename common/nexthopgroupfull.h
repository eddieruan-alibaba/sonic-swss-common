#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <netinet/in.h>
#include <cstdint>


namespace swss {

    /* Full nexthop group for zebra and fpm */
    struct nh_grp_full {
        std::uint32_t id;
        std::uint8_t weight;
        std::uint32_t num_direct;
    };

    /* VRF ID type */
    typedef std::uint32_t vrf_id_t;
    typedef signed int ifindex_t;

    union g_addr {
        struct in_addr ipv4;
        struct in6_addr ipv6;
    };

    enum nexthop_types_t {
        NEXTHOP_TYPE_INVALID = 0,   /* Default value */
        NEXTHOP_TYPE_IFINDEX = 1,   /* Directly connected */
        NEXTHOP_TYPE_IPV4,    /* IPv4 nexthop */
        NEXTHOP_TYPE_IPV4_IFINDEX,    /* IPv4 nexthop with ifindex */
        NEXTHOP_TYPE_IPV6,    /* IPv6 nexthop */
        NEXTHOP_TYPE_IPV6_IFINDEX,    /* IPv6 nexthop with ifindex */
        NEXTHOP_TYPE_BLACKHOLE,    /* Null0 nexthop */
    };

    /* LSP types. */
    enum lsp_types_t {
        ZEBRA_LSP_NONE = 0,   /* No LSP. */
        ZEBRA_LSP_STATIC = 1, /* Static LSP. */
        ZEBRA_LSP_LDP = 2,    /* LDP LSP. */
        ZEBRA_LSP_BGP = 3,    /* BGP LSP. */
        ZEBRA_LSP_OSPF_SR = 4,  /* OSPF Segment Routing LSP. */
        ZEBRA_LSP_ISIS_SR = 5,  /* IS-IS Segment Routing LSP. */
        ZEBRA_LSP_SHARP = 6,  /* Identifier for test protocol */
        ZEBRA_LSP_SRTE = 7,   /* SR-TE LSP */
        ZEBRA_LSP_EVPN = 8,  /* EVPN VNI Label */
    };

    enum blackhole_type {
        BLACKHOLE_UNSPEC = 0,
        BLACKHOLE_NULL,
        BLACKHOLE_REJECT,
        BLACKHOLE_ADMINPROHIB,
    };

    enum seg6local_action_t {
        SEG6_LOCAL_ACTION_UNSPEC       = 0,
        SEG6_LOCAL_ACTION_END          = 1,
        SEG6_LOCAL_ACTION_END_X        = 2,
        SEG6_LOCAL_ACTION_END_T        = 3,
        SEG6_LOCAL_ACTION_END_DX2      = 4,
        SEG6_LOCAL_ACTION_END_DX6      = 5,
        SEG6_LOCAL_ACTION_END_DX4      = 6,
        SEG6_LOCAL_ACTION_END_DT6      = 7,
        SEG6_LOCAL_ACTION_END_DT4      = 8,
        SEG6_LOCAL_ACTION_END_B6       = 9,
        SEG6_LOCAL_ACTION_END_B6_ENCAP = 10,
        SEG6_LOCAL_ACTION_END_BM       = 11,
        SEG6_LOCAL_ACTION_END_S        = 12,
        SEG6_LOCAL_ACTION_END_AS       = 13,
        SEG6_LOCAL_ACTION_END_AM       = 14,
        SEG6_LOCAL_ACTION_END_BPF      = 15,
        SEG6_LOCAL_ACTION_END_DT46     = 16,
    };

    struct seg6local_flavors_info {
        /* Flavor operations */
        std::uint32_t flv_ops;

        /* Locator-Block length, expressed in bits */
        std::uint8_t lcblock_len;
        /* Locator-Node Function length, expressed in bits */
        std::uint8_t lcnode_func_len;
    };

    struct seg6local_context {
        struct in_addr nh4;
        struct in6_addr nh6;
        std::uint32_t table;
        struct seg6local_flavors_info flv;
        std::uint8_t block_len;
        std::uint8_t node_len;
        std::uint8_t function_len;
        std::uint8_t argument_len;
    };

    /* SR Policy Headend Behaviors as per RFC 8986 section #5 */
    enum srv6_headend_behavior {
        SRV6_HEADEND_BEHAVIOR_H_INSERT,
        SRV6_HEADEND_BEHAVIOR_H_ENCAPS,
        SRV6_HEADEND_BEHAVIOR_H_ENCAPS_RED,
        SRV6_HEADEND_BEHAVIOR_H_ENCAPS_L2,
        SRV6_HEADEND_BEHAVIOR_H_ENCAPS_L2_RED,
    };

    struct seg6_seg_stack {
        enum srv6_headend_behavior encap_behavior;
        std::uint8_t num_segs;
        struct in6_addr seg[0]; /* 1 or more segs */
    };

    struct nexthop_srv6 {
        /* SRv6 localsid info for Endpoint-behaviour */
        enum seg6local_action_t seg6local_action = {};
        struct seg6local_context seg6local_ctx = {};

        /* SRv6 Headend-behaviour */
        struct seg6_seg_stack *seg6_segs = nullptr;
    };

    struct NextHopGroupFull {
        std::uint32_t id = 0;
        std::uint32_t key = 0;  /* Hash value from zebra for this nhg */
        std::uint8_t weight = 0;  /* Weight of the nexthop ( for unequal cost ECMP  ) */
        std::uint8_t flags = 0;
    #define NEXTHOP_FLAG_ONLINK     (1 << 3) /* Nexthop should be installed onlink */

        std::string ifname = "";  /* Interface name obtained from ifindex */
        std::vector<struct nh_grp_full> depends;
        std::vector<struct nh_grp_full> dependents;

        /* begin of hashed data - all fields from here onwards are given to
        * jhash() as one consecutive chunk.  DO NOT create "padding holes".
        * DO NOT insert pointers that need to be deep-hashed.
        *
        * static_assert() below needs to be updated when fields are added
        */
        char _hash_begin[0];

        enum nexthop_types_t type = NEXTHOP_TYPE_INVALID;    /* see above */
        vrf_id_t vrf_id = 0;    /* What vrf is this nexthop associated with? */
        ifindex_t ifindex = 0;    /* Interface index */
        enum lsp_types_t nh_label_type = ZEBRA_LSP_NONE;    /* Type of label(s), if any */

        /* padding: keep 16 byte alignment here */
        /* Nexthop address
        * make sure all 16 byte for IPv6 are zeroed when putting in an IPv4
        * address since the entire thing is hashed as-is
        */
        union {
            union g_addr gate;
            enum blackhole_type bh_type;
        };
        union g_addr src;
        union g_addr rmap_src; /* Src is set via routemap */

        /* end of hashed data - remaining fields in this struct are not
        * directly fed into jhash().  Most of them are actually part of the
        * hash but have special rules or handling attached.
        */
        char _hash_end[0];

        /* backup, labels and srv6 info are not included yet, let's add later */
        /* backup and label are not in consideration temporarily, let's add srv6 member */

        /* SRv6 information */
        struct nexthop_srv6 *nh_srv6 = nullptr;

        /* Constructor for multi-path NextHopGroupFull */
        NextHopGroupFull(std::uint32_t id_in, std::uint32_t key_in,
                    const std::vector<nh_grp_full>& depends_in,
                    const std::vector<nh_grp_full>& dependents_in);

        /* Constructor for singleton NextHopGroupFull */
        NextHopGroupFull(std::uint32_t id_in, std::uint32_t key_in, enum nexthop_types_t type_in,
                    vrf_id_t vrf_id_in, ifindex_t ifindex_in, std::string ifname_in,
                    enum lsp_types_t label_type_in, enum blackhole_type bh_type_in,
                    union g_addr gateway_in, union g_addr src_in, union g_addr rmap_src_in,
                    std::uint8_t weight_in, std::uint8_t flags_in, bool has_srv6, bool has_seg6_segs,
                    const struct nexthop_srv6* nh_srv6_in,
                    const struct seg6_seg_stack* nh_seg6_segs_in,
                    const std::vector<struct in6_addr>& nh_segs_in);

        /* Destructor of NextHopGroupFull */
        ~NextHopGroupFull();
    };
}
