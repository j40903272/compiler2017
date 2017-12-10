int CNT = 5;
int funct (int aa[], int bb[])
{
    int i,j,k;
    for (i=0; i<CNT; i++)
    {
        j+=aa[i];
        k+=bb[i];
    }
    return j+k;
}
int funct1 (int aa[][100], int bb[][100])
{
    int i,j,k;
    for (i=0; i<CNT; i++)
    {
        j+=aa[i][1];
        k+=bb[i][1];
    }
    return j+k;
}

int main ()
{
    float xy;
    int a[10][100];
    int b[10][100];
    int x,y,z;
    z=funct(a[1],b[1]);
    xy=funct(a[2],b[2]);
    xy=funct(a[1],b[2]);
    xy=funct1(a,b);
    xy=funct1(a,b);
}
