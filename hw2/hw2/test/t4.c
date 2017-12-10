int fact(int n)
{
	if(n==1)
	{
		return n;
	}
	else
	{
	return (n*fact(n-1));
	}
}

int main()
{
	int n, result;
	write("Enter a number");
	n = read();
	if(n > 1)
	{
	result = fact(n);
	}
	else
	{
		result = 1;
	}
	write("The factorial is");
	write(result);
}
