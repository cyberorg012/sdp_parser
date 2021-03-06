#ifndef _SDP_TEST_UTIL_H_
#define _SDP_TEST_UTIL_H_

#include <math.h>
#if defined(__linux__)
#include <errno.h>
#include <unistd.h>
#endif

#include "sdp_parser.h"
#include "sdp_field.h"

/******************************************************************************
                                 Common Stuff
******************************************************************************/

#ifndef ARRAY_SZ
#define ARRAY_SZ(array) (int)(sizeof(array) / sizeof(array[0]))
#endif

#define C_RED    "\033[00;31m"
#define C_GREEN  "\033[00;32m"
#define C_YELLOW "\033[00;33m"
#define C_ITALIC "\033[00;03m"
#define C_NORMAL "\033[00;00;00m"

int print_title(const char *format, ...);

/******************************************************************************
                                  Test Info
******************************************************************************/
typedef int (*test_func)(void);
struct single_test
{
	const char *name;
	const char *description;
	test_func func;
};

#define MAX_NUM_TESTS 300
extern struct single_test tests[MAX_NUM_TESTS];
extern int num_tests;

int test_log(const char *format, ...);
void add_test(test_func func, const char *name, const char *description);
void init_tests();

#define REG_TEST(_name_, _summary_) \
	static const char *_name_ ## _summary = _summary_; \
	static int _name_(void)

#define ADD_TEST(_name_) add_test(_name_, #_name_, _name_ ## _summary)

/******************************************************************************
                                Validator Info
******************************************************************************/
#define IGNORE_VALUE -1
#define SESSION_ATTR -1
#define MAX_NUM_MEDIAS 40
#define MAX_NUM_SESSION_ATTRIBUTES 20
#define MAX_NUM_MEDIA_ATTRIBUTES 20
#define MAX_NUM_ATTRIBUTE_FIELDS 10
#define MAX_NUM_MEDIA_FORMATS 10
#define MAX_NUM_GROUP_TAGS 10

struct tag_validator_info {
	const char *name;
	int media_id;
};

struct group_validator_info {
	const char *semantic;
	int tag_count;
	struct tag_validator_info tags[MAX_NUM_GROUP_TAGS];
};

typedef void (*sdp_attr_func_ptr)(void);

struct fmt_validator_info {
	int id;
	int sub_type;
};

struct attr_validator_info {
	sdp_attr_func_ptr func; /* Indicates validator type */
	struct interpretable args[MAX_NUM_ATTRIBUTE_FIELDS];
};

struct media_validator_info {
	int fmt_count;
	struct fmt_validator_info formats[MAX_NUM_MEDIA_FORMATS];
	int attr_count;
	struct attr_validator_info attributes[MAX_NUM_MEDIA_ATTRIBUTES];
};

struct session_validator_info {
	int media_count;
	int session_attr_count;
	struct attr_validator_info attributes[MAX_NUM_SESSION_ATTRIBUTES];
	struct media_validator_info medias[MAX_NUM_MEDIAS];
};

extern struct session_validator_info validator_info;

void init_session_validator(void);

/******************************************************************************
                              Common Validators
******************************************************************************/
int assert_error(const char *format, ...);
int assert_str(const char *left_name, const char *left, const char *right_name,
		const char *right);
int assert_int(const char *left_name, long long left, const char *right_name,
		long long right);
int assert_flt(const char *left_name, double left, const char *right_name,
		double right);
int assert_ptr(const char *left_name, void *left, const char *right_name,
		void *right);
int assert_res(int res, const char *name, const char *file, int line);

#define ASSERT_RES(_res_) \
	assert_res(_res_, #_res_, __FILE__, __LINE__)
#define ASSERT_STR(_left_, _right_) \
	assert_res(assert_str(#_left_, _left_, #_right_, _right_), \
		"ASSERT_STR(" #_left_ ", " #_right_ ")", __FILE__, __LINE__)
#define ASSERT_INT(_left_, _right_) \
	assert_res(assert_int(#_left_, _left_, #_right_, _right_), \
		"ASSERT_INT(" #_left_ ", " #_right_ ")", __FILE__, __LINE__)
#define ASSERT_FLT(_left_, _right_) \
	assert_res(assert_flt(#_left_, _left_, #_right_, _right_), \
		"ASSERT_FLT(" #_left_ ", " #_right_ ")", __FILE__, __LINE__)
#define ASSERT_PTR(_left_, _right_) \
	assert_res(assert_ptr(#_left_,_left_, #_right_, _right_), \
		"ASSERT_PTR(" #_left_ ", " #_right_ ")", __FILE__, __LINE__)

int assert_attr(struct sdp_session*, struct sdp_attr *attr,
		struct attr_validator_info *av);
int assert_session(struct sdp_session *session);
int test_generic(const char *content, enum sdp_parse_err expected,
		int (*verifier)(struct sdp_session *session),
		struct sdp_specific *specific);

#endif
