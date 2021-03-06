#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>
#include <string.h>

#include "rule_helper.h"

/* Wrap functions */
FILE *__wrap_fopen(char *filename)
{
    return mock_type(FILE *);
}

int __wrap_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return mock_type(int);
}

/********************/

/*
 * Cleanup function. Sets values of dynamically allocated struct to NULL
 */
void cleanup(struct Connection *conn) {
    if (conn) {
	if (conn->file) {
	    conn->file = NULL;
	}

	if (conn->rules) {
	    conn->rules = NULL;
	}

	conn = NULL;
    }
}

/* Tests */
static void set_policy_test_allow(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = ALLOW;

    char *policy_str = "ALLOW";
    actual = set_policy(policy_str);

    assert_int_equal(actual, expected);
}

static void set_policy_test_deny(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = DENY;

    char *policy_str = "DENY";
    actual = set_policy(policy_str);

    assert_int_equal(actual, expected);
}

static void set_policy_test_reject(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = REJECT;

    char *policy_str = "REJECT";
    actual = set_policy(policy_str);

    assert_int_equal(actual, expected);
}

static void set_policy_test_allow_ips(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = ALLOW_WITH_IPS;

    char *policy_str = "ALLOW with IPS";
    actual = set_policy(policy_str);

    assert_int_equal(actual, expected);
}

static void set_policy_test_invalid(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = 0;

    char *policy_str = "This is so wrong Carl";
    actual = set_policy(policy_str);

    assert_int_equal(actual, expected);
}

static void get_rules_num_test(void **state)
{
    (void) state; /* unused */
    int actual, expected;
    expected = 5;
    
    struct Connection *conn = malloc(sizeof(struct Connection));

    if (!conn) {
	printf("Memory error.\n");
	exit(1);
    }

    conn->rules = malloc(sizeof(struct Rules));

    if (!conn->rules) {
	printf("Memory error.\n");
	exit(1);
    }

    int i = 0;
    for (i = 0; i < MAX_RULES; i++) {
	struct Rule rule;
	if (i < expected) {
	    rule.set = 1;
	} else {
	    rule.set = 0;
	}

	conn->rules->rules[i] = rule;
    }

    actual = get_rules_num(conn);

    cleanup(conn);

    free(conn->rules);
    free(conn);

    assert_int_equal(actual, expected);
}

static void is_match_test_success(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 1;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, src);
    strcpy(rule->dst, dst);
    strcpy(rule->master_proto, master_proto);
    strcpy(rule->app_proto, app_proto);
    rule->dport = dport;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);

    assert_int_equal(actual, expected);
}
static void is_match_test_src_no_match(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 0;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, "192.168.0.1");
    strcpy(rule->dst, dst);
    strcpy(rule->master_proto, master_proto);
    strcpy(rule->app_proto, app_proto);
    rule->dport = dport;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void is_match_test_dst_no_match(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 0;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, src);
    strcpy(rule->dst, "10.10.10.10");
    strcpy(rule->master_proto, master_proto);
    strcpy(rule->app_proto, app_proto);
    rule->dport = dport;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void is_match_test_dport_no_match(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 0;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, src);
    strcpy(rule->dst, dst);
    strcpy(rule->master_proto, master_proto);
    strcpy(rule->app_proto, app_proto);
    rule->dport = 999;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void is_match_test_app_proto_no_match(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 0;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, src);
    strcpy(rule->dst, dst);
    strcpy(rule->master_proto, master_proto);
    strcpy(rule->app_proto, "Google");
    rule->dport = dport;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void is_match_test_all_any(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 1;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, "any");
    strcpy(rule->dst, "any");
    strcpy(rule->master_proto, "any");
    strcpy(rule->app_proto, "any");
    rule->dport = 0;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void is_match_test_master_proto_no_match(void **state)
{
    (void) state; /* unused */
    int expected, actual;
    char *src, *dst, *master_proto, *app_proto;
    unsigned short dport;
    
    expected = 0;
    src = "127.0.0.1";
    dst = "8.8.8.8";
    dport = 666;
    master_proto = "HTTPS";
    app_proto = "Facebook";

    struct Rule *rule = malloc(sizeof(*rule));
    rule->set = 1;
    strcpy(rule->src, src);
    strcpy(rule->dst, dst);
    strcpy(rule->master_proto, "DNS");
    strcpy(rule->app_proto, app_proto);
    rule->dport = dport;
    rule->policy = ALLOW;

    actual = is_match(rule, src, dst, dport, master_proto, app_proto);
    free(rule);
    
    assert_int_equal(actual, expected);
}

static void rule_delete_test(void **state)
{
    (void) state; /* unused */

    struct Connection *conn = malloc(sizeof(struct Connection));

    if (!conn) {
	printf("Memory error.\n");
	exit(1);
    }

    conn->rules = malloc(sizeof(struct Rules));

    if (!conn->rules) {
	printf("Memory error.\n");
	exit(1);
    }

    struct Rule expected_rule = { .set = 0 };
    struct Rule rule = { .set = 1 };

    conn->rules->rules[0] = rule;
    rule_delete(conn, 0);
    
    struct Rule actual_rule = conn->rules->rules[0];

    int expected = expected_rule.set;
    int actual = actual_rule.set;

    cleanup(conn);
    free(conn->rules);
    free(conn);
    
    assert_int_equal(actual, expected);
}

static void rules_get_test_success(void **state)
{
    (void) state; /* unused */

    struct Connection *conn = malloc(sizeof(struct Connection));

    if (!conn) {
	printf("Memory error.\n");
	exit(1);
    }

    conn->rules = malloc(sizeof(struct Rules));

    if (!conn->rules) {
	printf("Memory error.\n");
	exit(1);
    }

    struct Rules *actual = rules_get(conn);

    assert_non_null(actual);
    
    cleanup(conn);
    free(conn->rules);
    free(conn);
}

static void rules_get_test_fail(void **state)
{
    (void) state; /* unused */

    struct Connection *conn = malloc(sizeof(struct Connection));

    if (!conn) {
	printf("Memory error.\n");
	exit(1);
    }

    struct Rules *actual = rules_get(conn);

    assert_null(actual);

    cleanup(conn);
    free(conn);
}

static void rule_set_test_success(void **state)
{
    (void) state; /* unused */

    struct Connection *conn = malloc(sizeof(struct Connection));
    if (!conn) {
	printf("Memory error.\n");
	exit(1);
    }
    
    conn->rules = malloc(sizeof(struct Rules));
    if (!conn->rules) {
	printf("Memory error.\n");
	exit(1);
    }

    // set expected values
    struct Rule *expected_rule = malloc(sizeof(*expected_rule));
    expected_rule->set = 1;
    strcpy(expected_rule->src, "any");
    strcpy(expected_rule->dst, "any");
    strcpy(expected_rule->master_proto, "any");
    strcpy(expected_rule->app_proto, "any");
    expected_rule->dport = 100;
    expected_rule->policy = ALLOW;

    rule_set(conn, 0, "any", "any", 100, "any", "any", ALLOW);

    // retrieve actual values
    struct Rule *actual_rule = &conn->rules->rules[0];

    assert_memory_equal(expected_rule, actual_rule, sizeof(struct Rule));
    
    cleanup(conn);
    free(expected_rule);
    free(conn->rules);
    free(conn);
}

static void rules_create_test(void **state)
{
    (void) state; /* unused */

    struct Connection *expected_conn = malloc(sizeof(struct Connection));
    struct Connection *actual_conn = malloc(sizeof(struct Connection));
    if (!expected_conn || !actual_conn) {
	printf("Memory error.\n");
	exit(1);
    }
    
    expected_conn->rules = malloc(sizeof(struct Rules));
    actual_conn->rules = malloc(sizeof(struct Rules));
    if (!expected_conn->rules || !actual_conn) {
	printf("Memory error.\n");
	exit(1);
    }
    
    int i = 0;
    for (i = 0; i < MAX_RULES; i++) {
	struct Rule rule = { .set = 0 };
	expected_conn->rules->rules[i] = rule;
    }

    rules_create(actual_conn);

    assert_memory_equal(expected_conn->rules, actual_conn->rules, sizeof(struct Rules));
    
    cleanup(expected_conn);
    cleanup(actual_conn);

    free(expected_conn->rules);
    free(expected_conn);
    free(actual_conn->rules);
    free(actual_conn);
}

static void rules_open_test_read(void **state)
{
    (void) state; /* unused */

    FILE *file = malloc(sizeof(*file));
    will_return(__wrap_fopen, file);
    will_return(__wrap_fread, 1);

    struct Connection *conn = rules_open("file", 'j');
    assert_non_null(conn);
    assert_non_null(conn->rules);
    assert_non_null(conn->file);

    free(file);
    cleanup(conn);
    free(conn->file);
    free(conn->rules);
    free(conn);
}

static void rules_open_test_create(void **state)
{
    (void) state; /* unused */

    FILE *file = malloc(sizeof(*file));
    will_return(__wrap_fopen, file);

    struct Connection *conn = rules_open("file", 'c');
    assert_non_null(conn);
    assert_non_null(conn->rules);
    assert_non_null(conn->file);

    free(file);
    cleanup(conn);
    free(conn->file);
    free(conn->rules);
    free(conn);
}

static void parse_string_test_success(void **state)
{
    (void) state; /* unused */

    char *pattern = "\\d+\\.\\d+\\.\\d+\\.\\d+";
    char *string = "192.293.44.1";

    char **expected = malloc(sizeof(char *));
    expected[0] = malloc(strlen(string) * sizeof(char));
    expected[0] = string;

    char **actual = parse_string(string, pattern, 0);

    assert_string_equal(expected[0], actual[0]);

    free(actual);
    free(expected);
}

static void parse_string_test_fail(void **state)
{
    (void) state; /* unused */

    char *pattern = "\\d+\\.\\d+\\.\\d+\\.\\d+";
    char *string = "123.2e3.44.1";

    char **actual = parse_string(string, pattern, 0);

    assert_null(actual);
}

static void parse_string_test_groups(void **state)
{
    (void) state; /* unused */

    char *pattern = "(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)";
    char string[] = "192.293.44.1";

    char **expected = malloc(4 * sizeof(char *));
    int i = 0;
    for (i = 0; i < 4; i++) {
	expected[i] = malloc(strlen(string) * sizeof(char));
    }

    expected[0] = "192";
    expected[1] = "293";
    expected[2] = "44";
    expected[3] = "1";

    char **actual = parse_string(string, pattern, 0);

    assert_string_equal(actual[1], expected[0]);
    assert_string_equal(actual[2], expected[1]);
    assert_string_equal(actual[3], expected[2]);
    assert_string_equal(actual[4], expected[3]);

    free(actual);
    free(expected);
}


int main(void) {
    const struct CMUnitTest tests[] = {
    cmocka_unit_test(parse_string_test_groups),
    cmocka_unit_test(parse_string_test_fail),
    cmocka_unit_test(parse_string_test_success),
    cmocka_unit_test(set_policy_test_allow),
    cmocka_unit_test(set_policy_test_deny),
    cmocka_unit_test(set_policy_test_reject),
    cmocka_unit_test(set_policy_test_allow_ips),
    cmocka_unit_test(set_policy_test_invalid),
    cmocka_unit_test(get_rules_num_test),
    cmocka_unit_test(is_match_test_all_any),
    cmocka_unit_test(is_match_test_app_proto_no_match),
    cmocka_unit_test(is_match_test_master_proto_no_match),
    cmocka_unit_test(is_match_test_dport_no_match),
    cmocka_unit_test(is_match_test_dst_no_match),
    cmocka_unit_test(is_match_test_src_no_match),
    cmocka_unit_test(is_match_test_success),
    cmocka_unit_test(rule_delete_test),
    cmocka_unit_test(rules_get_test_success),
    cmocka_unit_test(rules_get_test_fail),
    cmocka_unit_test(rule_set_test_success),
    cmocka_unit_test(rules_create_test),
    cmocka_unit_test(rules_open_test_read),
    cmocka_unit_test(rules_open_test_create)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
