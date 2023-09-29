/// @file stack.cpp

#include "stack.h"

/**
 * types of errors
*/

static const char *err_msgs_arr[] = {
    "NO ERROR.\n",
    "ERROR: null pointer to stack.\n",
    "ERROR: null pointer to stack data.\n",
    "ERROR: wrong stack size.\n",
    "ERROR: wrong stack position.\n",
    "ERROR: stack size is less than position.\n",
    "ERROR: error when opening file.\n",
    "ERROR: error when closing file.\n",
    "ERROR: your stack is full of shit, call ctor.\n",
    "ERROR: hash->struct does not match true value.\n",
    "ERROR: hash->data does not match true value.\n",
    "ERROR: error when checking stack canary.\n",
    "ERROR: error when checking stack left_canary\n",
    "ERROR: error when checking stack right_canary\n",
    "ERROR: error when checking stack data->canary\n",
    "ERROR: error when checking stack data->left_canary\n",
    "ERROR: error when checking stack data->right_canary\n"
};

static const char *fp_err_name = "file_err.txt";                                                                        ///< Variable storing the file name for error output.

/**
 * Function to create a stack.
 * @param[in] stk
 * @param[in] capacity
*/

void stack_ctor (STACK *stk, size_t capacity)
{
    my_assert (stk != NULL);
    my_assert (stk->data == NULL);

#ifdef CANARIES_CHECK
    stk->data = (ELEMENT *) calloc (capacity * sizeof (ELEMENT) + 2 * sizeof (CANARY_TYPE), sizeof (char));
    my_assert (stk->data != NULL)

    *((CANARY_TYPE *) stk->data) = CANARY;
    *((CANARY_TYPE *)(stk->data + capacity + 1)) = CANARY;
    stk->data = (ELEMENT *) stk->data;
    stk->data++;

    stk->left_canary = CANARY;
    stk->right_canary = CANARY;
#else
    stk->data = (ELEMENT *) calloc  (capacity * sizeof (ELEMENT), sizeof (ELEMENT));
    my_assert (stk->data != NULL);
#endif

    memset (stk->data, 0, capacity * sizeof (ELEMENT));

    stk->size = capacity;
    stk->position = 0;

#ifdef HASH_CHECK
    stk->hash_data = hash_control_data (stk);
    stk->hash_struct = hash_control_struct (stk);
#endif

    assert_stack (stk);
}

/**
 * Stack cleaning function.
 * @param[in] stk
*/

void stack_dtor (STACK *stk)
{
    my_assert (stk->data != NULL);
    assert_stack (stk);

    free (stk->data);

#ifdef CANARIES_CHECK
    stk->left_canary  = STACK_VALUE_VENOM;
    stk->right_canary = STACK_VALUE_VENOM;
#endif

#ifdef HASH_CHECK
    stk->hash_data   = STACK_VALUE_VENOM;
    stk->hash_struct = STACK_VALUE_VENOM;
#endif

    stk->data = NULL;
    stk->size = STACK_VALUE_VENOM;
    stk->position = STACK_VALUE_VENOM;
}

/**
 * Function to add a new element to the stack.
 * @param[in] stk
 * @param[in] value
*/

void stack_push (STACK *stk, ELEMENT value)
{
    assert_stack (stk);

    if (stk->size == stk->position)
    {
        stack_realloc (stk, stk->size * 2);
    }

    stk->data[stk->position++] = value;

#ifdef HASH_CHECK
    stk->hash_data = hash_control_data (stk);
    stk->hash_struct = hash_control_struct (stk);
#endif

    assert_stack (stk);
}

/**
 * Function that returns the last element added.
 * @param[in] stk
*/

ELEMENT stack_pop (STACK *stk)
{
    if (stk->position < stk->size / 3)
    {
        stack_realloc (stk, stk->position);
    }

    ELEMENT elem_pop = stk->data[--stk->position];

    stk->data[stk->position] = 0;

#ifdef HASH_CHECK
    stk->hash_data = hash_control_data (stk);
    stk->hash_struct = hash_control_struct (stk);
#endif

    assert_stack (stk);

    return elem_pop;
}

/**
 * A function that changes the stack size.
 * @param[in] stk
 * @param[in] size
*/

void stack_realloc (STACK *stk, int size)
{
    assert_stack (stk);

#ifdef CANARIES_CHECK
    stk->data = (ELEMENT *) realloc (stk->data - 1, size * sizeof (ELEMENT) + 2 * sizeof (CANARY_TYPE));
    my_assert (stk->data != NULL);

    stk->data++;

    *((CANARY_TYPE *) (stk->data + size)) = CANARY;
#else
    stk->data = (ELEMENT *) realloc (stk->data, size * sizeof (ELEMENT));
    my_assert (stk->data != NULL);
#endif

    if (size - stk->size > 0)
    {
        memset (stk->data + stk->size, 0, (size - stk->size) * sizeof (ELEMENT));
    }

    stk->size = size;

#ifdef HASH_CHECK
    stk->hash_data = hash_control_data (stk);
    stk->hash_struct = hash_control_struct (stk);
#endif

    assert_stack (stk);
}

#ifdef HASH_CHECK

/**
 * Function that creates a hash of an array of stack elements.
 * @param[in] stk
 * @param[out] hash_data
*/

HASH_TYPE hash_control_data (STACK *stk)
{
    my_assert (stk != NULL);

    HASH_TYPE hash_data = 0;

    for (int i = 0; i < stk->position; i++)
    {
        hash_data += hash_djb (stk->data[i]);
    }

    return hash_data;
}

/**
 * Function that creates a hash of a structure.
 * @param[in] stk
 * @param[out] hash
*/

HASH_TYPE hash_control_struct (STACK *stk)
{
    my_assert (stk != NULL);

    int data = 0;

    for (int i = 0; i < stk->position; i++)
    {
        data += stk->data[i];
    }

    HASH_TYPE hash = hash_djb (data + stk->position + stk->size);

    return hash;
}

/**
 * Hash generating function.
 * @param[in] str
 * @param[out] hash
*/

HASH_TYPE hash_djb(int str)
{
    unsigned long hash = 5381;

    for (int i = 0; i < str; i++)
    {
        hash = ((hash << 5) + hash) + i;
    }

    return hash;
}
#endif

/**
 * Function that checks the stack for errors.
 * @param[in] stk
 * @param[out] code_error
*/

int stack_verification (STACK *stk)
{
    if (stk == NULL)
    {
        return STACK_ERR;
    }

    if (stk->position == STACK_VALUE_VENOM && stk->size == STACK_VALUE_VENOM)
    {
        return STACK_POINTER_GARBAGE;
    }

#ifdef HASH_CHECK
    if (stk->hash_struct != hash_control_struct (stk))
    {
        return STACK_HASH_STRUCT_ERR;
    }

    if (stk->hash_data != hash_control_data (stk))
    {
        return STACK_HASH_DATA_ERR;
    }
#endif

#ifdef CANARIES_CHECK
    if (stk->left_canary != CANARY && stk->right_canary != CANARY)
    {
        return STACK_CANARY_ERR;
    }

    if (stk->right_canary != CANARY)
    {
        return STACK_RIGHT_CANARY_ERR;
    }
#endif

    if (stk->data == NULL)
    {
        return STACK_DATA_ERR;
    }

    if (stk->position < 0)
    {
        return STACK_POSITION_ERR;
    }

    if (stk->size <= 0)
    {
        return STACK_SIZE_ERR;
    }

    if (stk->size < stk->position)
    {
        return STACK_SIZE_POS_ERR;
    }

#ifdef CANARIES_CHECK
    if (stk->data[-1] != (int) CANARY && stk->data[stk->size] != (int) CANARY)
    {
        return STACK_DATA_CANARY_ERR;
    }

    if (stk->data[-1] != (int) CANARY)
    {
        return STACK_DATA_LEFT_CANARY_ERR;
    }

    if (stk->data[stk->size] != (int) CANARY)
    {
        return STACK_DATA_RIGHT_CANARY_ERR;
    }
#endif
    

    return STACK_OK;
}

/**
 * Function that prints error and stack information.
 * @param[in] stk
 * @param[in] code_error
 * @param[in] file_err
 * @param[in] func_err
 * @param[in] line_err
*/

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err)
{
    FILE *fp_err = fopen (fp_err_name, "a");

    if (fp_err == NULL)
    {
        fprintf (stderr, "%s", err_msgs_arr[FILE_OPEN_ERR]);
    }
    
    if (stk != NULL)
    {
        if (code_error < ERROR_CNT)
        {
            fprintf (fp_err, "%s\n", err_msgs_arr[code_error]);
        }
        else
        {
            fprintf (fp_err, "Unknown error.\n\n");
        }

        fprintf (fp_err, "stack[%p] \"stk\" called from %s(%d) %s\n", stk, file_err, line_err, func_err);

        fprintf (fp_err, "{\n");

#ifdef CANARIES_CHECK
        fprintf (fp_err, "\tstack->left_canary = %x\n", stk->left_canary);
#endif

        fprintf (fp_err, "\tposition = %d\n", stk->position);
        fprintf (fp_err, "\tsize = %d\n", stk->size);

        if (stk->data != NULL)
        {
            fprintf (fp_err, "\tdata[%p]\n", stk->data);
            fprintf (fp_err, "\t{\n");

#ifdef CANARIES_CHECK
            fprintf (fp_err, "\t\tdata->left_canary = %llx\n", stk->data[-1]);
#endif

            if (stk->size > 0)
            {
                for (int i = 0; i < stk->size; i++)
                {
                    fprintf (fp_err, "\t\t*[%d] = %d\n", i, stk->data[i]);
                }
            }
            else if (stk->position > 0)
            {
                for (int i = 0; i < stk->position; i++)
                {
                    fprintf (fp_err, "\t\t*[%d] = %d\n", i, stk->data[i]);
                }
            }

#ifdef CANARIES_CHECK
            fprintf (fp_err, "\t\tdata->right_canary = %llx\n", stk->data[stk->size]);
#endif

            fprintf (fp_err, "\t}\n");
        }
        else 
        {
            fprintf (fp_err, "\tdata[NULL]\n");
        }

#ifdef HASH_CHECK
        fprintf (fp_err, "\tstack->hash_struct = %llu\n", stk->hash_struct);
        fprintf (fp_err, "\tstack->hash_data = %llu\n", stk->hash_data);
#endif

#ifdef CANARIES_CHECK
        fprintf (fp_err, "\tstack->right_canary = %llx\n", stk->right_canary);
#endif

        fprintf (fp_err, "}\n\n-----------------------------------------------------------\n");

        if (fclose (fp_err) != 0)
        {
            fprintf (fp_err, "%s", err_msgs_arr[FILE_CLOSE_ERR]);
        }
    }
    else
    {
        if (code_error < ERROR_CNT)
        {
            fprintf (fp_err, "%s\n", err_msgs_arr[code_error]);
        }
        else
        {
            fprintf (fp_err, "Unknown error.\n\n");
        }

        fprintf (fp_err, "stack[NULL] \"stk\" called from %s(%d) %s\n", file_err, line_err, func_err);
    }
}