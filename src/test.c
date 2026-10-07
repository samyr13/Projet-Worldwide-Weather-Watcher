#include <stdio.h>
#include <stdint.h>

uint8_t fibonacci_8bits_simu(uint8_t iterations)
{
    uint32_t a = 1;
    uint32_t b = 2;
    uint32_t r = 0;

    for (uint8_t i = 0; i < iterations; i++)
    {
        r = a + b;
        a = b;
        b = r;

        printf("%lu\r\n", r);
    }

    return (uint8_t)r;
}

uint32_t fibonacci_32bits_c(uint8_t iterations)
{
    uint32_t a = 1;
    uint32_t b = 2;
    uint32_t r = 0;

    for (uint8_t i = 0; i < iterations; i++)
    {
        r = a + b;
        a = b;
        b = r;

        printf("%lu\r\n", r);
    }

    return r;
}

int main(void)
{
    fibonacci_8bits_simu(11); 

    fibonacci_32bits_c(11);   

    return 0;
}


