int MAIN()
{
  int i, num1;
  int result = 0;
  
  write("enter a integer");
  num1 = read();
 
  for (i = 0; i < num1; i = i + 1)
    {
      result = result + i;
    } 
  write(result);  
  return 0; 
}

