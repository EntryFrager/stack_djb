#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define CANARIES_CHECK

#define HASH_CHECK

#ifdef CANARIES_CHECK
    typedef unsigned long long CANARY_TYPE;

    const CANARY_TYPE CANARY = 0xDEADBEEF;
#endif

#ifdef HASH_CHECK
    typedef unsigned long long HASH_TYPE;
#endif

static int CODE_ERROR = 0;

#define assert_stack(stack) { \
    if ((CODE_ERROR = stack_verification (stack)) != STACK_OK) \
    {   \
        stack_dump (stack, CODE_ERROR, __FILE__, __func__, __LINE__); \
        abort (); \
    }   \
}

#define my_assert(expr) if (!(expr)) {  \
    fprintf(stderr, "%s %s:%d: My assertion failed: \"" #expr "\"\n", __FILE__, __func__, __LINE__); \
    exit(1); \
}

typedef int ELEMENT;

const ELEMENT STACK_VALUE_VENOM = -100000;

typedef struct {
#ifdef CANARIES_CHECK
    CANARY_TYPE left_canary = CANARY;
#endif

    ELEMENT *data = NULL;

    int size = STACK_VALUE_VENOM;
    int position = STACK_VALUE_VENOM;

#ifdef HASH_CHECK
    HASH_TYPE hash_data = STACK_VALUE_VENOM;
    HASH_TYPE hash_struct = STACK_VALUE_VENOM;
#endif

#ifdef CANARIES_CHECK
    CANARY_TYPE right_canary = CANARY;
#endif
} STACK;

enum code_error
{
    STACK_OK,
    STACK_ERR,
    STACK_DATA_ERR,
    STACK_SIZE_ERR,
    STACK_POSITION_ERR,
    STACK_SIZE_POS_ERR,
    FILE_OPEN_ERR,
    FILE_CLOSE_ERR,
    STACK_POINTER_GARBAGE,

    STACK_HASH_STRUCT_ERR,
    STACK_HASH_DATA_ERR,
    STACK_CANARY_ERR,   
    STACK_LEFT_CANARY_ERR,
    STACK_RIGHT_CANARY_ERR,
    STACK_DATA_CANARY_ERR,   
    STACK_DATA_LEFT_CANARY_ERR,
    STACK_DATA_RIGHT_CANARY_ERR,
};

const int ERROR_CNT = 16;

void stack_ctor (STACK *stk, size_t capacity);

void stack_dtor (STACK *stk);

void stack_push (STACK *stk, ELEMENT value);

ELEMENT stack_pop (STACK *stk);

void stack_realloc (STACK *stk, int size);

#ifdef HASH_CHECK
HASH_TYPE hash_control_data (STACK *stk);

HASH_TYPE hash_control_struct (STACK *stk);

HASH_TYPE djb(int str);
#endif

int stack_verification (STACK *stk);

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err);

#endif // STACK_H