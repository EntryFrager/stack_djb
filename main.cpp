/// @file main.cpp

#include "stack.h"

/**
 * Main entry point of the program.
*/

int main ()
{
    STACK stack = {};

    stack_ctor (&stack, 5);

    assert_stack (&stack);

    for (int i = 0; i < 7; i++)
    {
        stack_push (&stack, (ELEMENT) i);
    }

    assert_stack (&stack);

    for (int i = 0; i < 7; i++)
    {
        ELEMENT stack_elem = stack_pop(&stack);

        printf ("%d\n", stack_elem);
    }

    assert_stack (&stack);

    stack_dtor (&stack);

    return 0;
}