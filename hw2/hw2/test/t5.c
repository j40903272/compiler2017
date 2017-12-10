/* Test0 */
/* This is only a test for the incomplete CFG */

int MatrixMultiplication(int V1[10], int V2[10])
{
    int result[10];         

    /* A sample for loop */
    int j;
    for(j = 0; j < 10; j = j + 1) 
    {
        result[j] = result[j] + V1[j]*V2[j];
    }

    return 1;
}

int main()
{
    /* typedef construct used here */
    typedef int COUNTERS;
    COUNTERS i;

    /* float used here, also the variable name is mixed with digits and 
       underscore*/
    float someReal12345 = 3.2e5 + 4.4;

    /* int declared here */
    int a,b,n;
    int bb[10];

    a = b + n;
    i = a - 1;
    bb[1] = i;
    /* Read, Fread, and Write are library functions, Can you support them? 
       write("Input some real number:");
       n = read();
       someReal12345 = fread();
       */

    /* While is used like this 
       a = 200000;
       while( n != 0) 
       {
       a = a/n;
       n = n - 1;
       }

       MatrixMultiplication(bb, bb);
       */

    /* If statements are as follows, can you support them? */

    return 0; 
}
