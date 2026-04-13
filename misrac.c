/* Example: MISRA C compliant C program */

#include <stdint.h>  /* For fixed-width integer types */
#include <stddef.h>  /* For size_t */

/* Define constants using const keyword */
const uint8_t MaxBufferSize = 256;

/* Function to initialize an array with zeros */
void InitializeBuffer(uint8_t *buffer, size_t size)
{
    size_t index;

    /* Ensure pointer is not NULL */
    if (buffer == NULL)
    {
        /* Handle error: In embedded systems, could set an error flag */
        return;
    }

    /* Initialize buffer elements to zero */
    for (index = 0U; index < size; index++)
    {
        buffer[index] = 0U;
    }
}

/* Main function */
int main(void)
{
    /* Declare buffer with fixed size */
    uint8_t buffer[MaxBufferSize];

    /* Initialize buffer */
    InitializeBuffer(buffer, MaxBufferSize);

    /* Application code here */

    return 0;
}


/*

misrac.c:7:31: warning: unsigned conversion from ‘int’ to ‘uint8_t’ {aka ‘const unsigned char’} changes value from ‘256’ to ‘0’ [-Woverflow]
    7 | const uint8_t MaxBufferSize = 256;


*/