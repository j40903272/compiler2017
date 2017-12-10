/*this is a comment*/

int main()
{
	int n, sum;
	int loopvar;
	write("What is n:");
	n = read();
	sum = 0;
	for(loopvar = 1; loopvar <= n; loopvar = loopvar +1)
	{
		sum = sum + loopvar;
	}
	write("The sum is:");
	write(sum);
}
