int func(int a, int b, int c, int d)
{
  if (a>1){
    return a*b-3;
  }
  else{
    return d*(c-2)-3;
  }
}

int MAIN()
{
  int a=1,b=2,c;
  int result=1;
  
  c=a*b;
  result=func(a - 1, b * 2, c, a-c)*result;
  
  write(result);
  write("\n");
  return 0;
}

