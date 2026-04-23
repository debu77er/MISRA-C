/* Example: MISRA-C compliant factorial calculator in C for Linux */

#include <stdio.h>
#include <stdlib.h>

/* Define maximum input to prevent overflow and excessive computation */
#define MAX_INPUT 12

/* Function to compute factorial iteratively */
static unsigned long long factorial(unsigned int number)
{
    unsigned int index = 0U;
    unsigned long long result = 1ULL;

    for (index = 1U; index <= number; ++index)
    {
        result *= (unsigned long long)index;
    }

    return result;
}

/* Main function */
int main(int argc, char *argv[])
{
    unsigned int input_number = 0U;
    unsigned long long fact_result = 0ULL;

    /* Check for correct number of arguments */
    if (argc != 2)
    {
        /* Print usage message */
        printf("Usage: %s <non-negative integer>\n", argv[0]);
        return 1;
    }

    /* Convert input argument to integer */
    char *endptr = NULL;
    long temp_value = 0L;

    temp_value = strtol(argv[1], &endptr, 10);

    /* Validate conversion */
    if ((endptr == argv[1]) || (*endptr != '\0'))
    {
        /* Invalid input string */
        printf("Error: Input is not a valid integer.\n");
        return 1;
    }

    /* Check for non-negative input and range */
    if (temp_value < 0L || temp_value > (long)MAX_INPUT)
    {
        printf("Error: Input must be between 0 and %d.\n", MAX_INPUT);
        return 1;
    }

    input_number = (unsigned int)temp_value;

    /* Compute factorial */
    fact_result = factorial(input_number);

    /* Print result */
    printf("Factorial of %u is %llu.\n", input_number, fact_result);

    return 0;
}

/*

Key considerations for MISRA C compliance:

    Explicit types: Using fixed-width types like unsigned int and unsigned long long.
    Avoiding dynamic memory: No malloc or free.
    Controlled flow: Clear if-else structures and return statements.
    No implicit conversions: Explicit casts used.
    Function static: Helper functions are declared static to limit scope.
    Constants: Using #define for constants.
    Input validation: Proper validation of user input.

Additional steps for full MISRA compliance:

    Use static analysis tools configured for MISRA C.
    Avoid using non-standard library functions if possible.
    Follow naming conventions and formatting rules.
    Handle all possible error conditions explicitly.

Disclaimer: This example is simplified for illustration purposes. 
Full MISRA C compliance may require more rigorous practices, 
detailed documentation, and static analysis tools.

Creating a Linux C application that complies with MISRA C rules is a complex task, 
as MISRA C guidelines emphasize safety, portability, and maintainability, often 
requiring careful coding practices. Below is a simple example of a Linux C 
application that adheres to many MISRA C principles, such as avoiding undefined 
behavior, using explicit types, avoiding dynamic memory when unnecessary, and 
ensuring clear control flow.

This example will implement a basic program that reads an integer from the command 
line, computes its factorial, and prints the result. It avoids dynamic memory 
allocation, uses explicit types, and handles errors gracefully.

Note: While this example follows many MISRA C rules, full compliance requires 
rigorous static analysis and might involve additional configuration depending 
on the MISRA C standard version (e.g., 1998, 2004, 2012).

*/



