#include "test.h"
#include <qlite.h>
#include <string.h>

/* ---- ql_cid_generate ---- */

TEST(test_cid_generate_sets_len) {
    ql_cid_t cid;
    ql_cid_generate(&cid, 8);
    EXPECT_EQ(cid.len, 8);
}

TEST(test_cid_generate_max_len) {
    ql_cid_t cid;
    ql_cid_generate(&cid, QL_CID_MAX_LEN);
    EXPECT_EQ(cid.len, QL_CID_MAX_LEN);
}

TEST(test_cid_generate_zero_len) {
    ql_cid_t cid;
    memset(&cid, 0xAB, sizeof(cid)); /* poison first */
    ql_cid_generate(&cid, 0);
    EXPECT_EQ(cid.len, 0);
}

TEST(test_cid_generate_rejects_over_max_len) {
    ql_cid_t cid;
    memset(&cid, 0xCD, sizeof(cid));
    ql_cid_generate(&cid, QL_CID_MAX_LEN + 1);
    /* struct must be left untouched on rejection */
    EXPECT_EQ(cid.len, 0xCD);
    EXPECT_EQ(cid.data[0], 0xCD);
}

TEST(test_cid_generate_null_ptr_does_not_crash) {
    ql_cid_generate(NULL, 8); /* must not segfault */
}

TEST(test_cid_generate_two_calls_differ) {
    ql_cid_t a, b;
    ql_cid_generate(&a, QL_CID_MAX_LEN);
    ql_cid_generate(&b, QL_CID_MAX_LEN);
    /* astronomically unlikely to collide if RNG is working */
    EXPECT_NE(memcmp(a.data, b.data, QL_CID_MAX_LEN), 0);
}

TEST(test_cid_generate_zeroes_unused_tail_bytes) {
    ql_cid_t cid;
    memset(&cid, 0xFF, sizeof(cid));
    ql_cid_generate(&cid, 5);
    EXPECT_EQ(cid.len, 5);
    for (int i = 5; i < QL_CID_MAX_LEN; i++) {
        EXPECT_EQ(cid.data[i], 0);
    }
}

TEST(test_cid_generate_not_all_zero_bytes) {
    /* sanity check the RNG actually produced entropy, not silently
     * left the buffer zeroed */
    ql_cid_t cid;
    ql_cid_generate(&cid, QL_CID_MAX_LEN);
    uint8_t all_zero[QL_CID_MAX_LEN] = {0};
    EXPECT_NE(memcmp(cid.data, all_zero, QL_CID_MAX_LEN), 0);
}

/* ---- ql_cid_cmp ---- */

TEST(test_cid_cmp_equal_cids) {
    ql_cid_t a, b;
    ql_cid_generate(&a, 10);
    b = a;
    EXPECT_EQ(ql_cid_cmp(&a, &b), 0);
}

TEST(test_cid_cmp_self) {
    ql_cid_t a;
    ql_cid_generate(&a, 10);
    EXPECT_EQ(ql_cid_cmp(&a, &a), 0);
}

TEST(test_cid_cmp_different_content_same_len) {
    ql_cid_t a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    a.len = b.len = 8;
    a.data[0]     = 0x01;
    b.data[0]     = 0x02;
    EXPECT_NE(ql_cid_cmp(&a, &b), 0);
}

TEST(test_cid_cmp_different_lengths_never_equal) {
    ql_cid_t a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    a.len = 8;
    b.len = 9;
    /* identical byte content, differing len */
    EXPECT_NE(ql_cid_cmp(&a, &b), 0);
}

TEST(test_cid_cmp_zero_length_cids_equal) {
    ql_cid_t a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0xFF, sizeof(b)); /* garbage data[], but len=0 */
    a.len = 0;
    b.len = 0;
    /* len=0 means no bytes are compared; must be equal regardless
     * of leftover/garbage bytes in data[] */
    EXPECT_EQ(ql_cid_cmp(&a, &b), 0);
}

TEST(test_cid_cmp_differs_only_in_last_byte) {
    ql_cid_t a, b;
    memset(&a, 0x42, sizeof(a));
    memset(&b, 0x42, sizeof(b));
    a.len = b.len = QL_CID_MAX_LEN;
    a.data[QL_CID_MAX_LEN - 1] ^= 0x01;
    EXPECT_NE(ql_cid_cmp(&a, &b), 0);
}

TEST(test_cid_cmp_null_args) {
    ql_cid_t a;
    ql_cid_generate(&a, 8);
    EXPECT_NE(ql_cid_cmp(NULL, &a), 0);
    EXPECT_NE(ql_cid_cmp(&a, NULL), 0);
    EXPECT_NE(ql_cid_cmp(NULL, NULL), 0);
}

TEST(test_cid_cmp_max_len_roundtrip) {
    ql_cid_t a, b;
    ql_cid_generate(&a, QL_CID_MAX_LEN);
    b = a;
    EXPECT_EQ(ql_cid_cmp(&a, &b), 0);
    b.data[3] ^= 0xFF;
    EXPECT_NE(ql_cid_cmp(&a, &b), 0);
}