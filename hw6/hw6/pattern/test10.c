float pi=3.1416;

float sqr(float num)
{
  return (num*num);
}

float calarea(int r)
{
  float area;
  area = pi * sqr(r);
  return area;
}

int floor(float num)
{
  int temp;
  temp = num;
  return temp;
}

int MAIN(){
  
  int r;
  float area;
  float rem;
  write("Enter an Integer :"); 
  r=read();

  area=calarea(r);
  rem=area-floor(area);

  write(area);
  write(" ");
  write(rem);
  write("\n");

  return 0;
}
