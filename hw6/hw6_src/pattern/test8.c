int dim = 4;

int a[4][4], b[4][4];
int c[4][4];

void print()
{
  int i,j;
  for (i = 0; i < dim; i = i + 1)
    {
      for (j = 0; j < dim; j = j + 1)
	{
	  write(c[i][j]);
          write(" ");
	}
      write("\n"); 
    }
}



void arraymult()
{
  int i,j,k;
  for (i = 0; i < dim; i = i + 1)
    {
      for (j = 0; j < dim; j = j + 1)
	{
	  int sum = 0;
	  for (k = 0; k < dim; k = k + 1)
	    {
	      sum = sum + a[i][k]*b[k][j];
	    }
	  c[i][j] = sum;
	}
    }
  print();
}


int MAIN()
{
  int i,j;
  write("Enter matrix 1 of dim 4 x 4 : \n");

  for (i = 0; i < dim; i = i +1)
    {
      for (j = 0; j < dim; j = j +1)
	{
	  a[i][j] = read();
	}
    }
  write("Enter matrix 2 of dim 4 x 4 : \n");
  for (i = 0; i < dim; i = i +1)
    {
      for (j = 0; j < dim; j = j +1)
	{
	  b[i][j] = read();
	}
    }
  arraymult();

  return 0;
}


