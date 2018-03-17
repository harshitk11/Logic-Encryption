#include <stdio.h>

int main(void)
{

	for(int i = 1; i<=64;i=i+4)
	{	
		printf("%d-> R\n%d-> R\n",i,i+1);

	}

	for(int i = 3; i<=64;i=i+4)
	{	
		printf("%d-> B\n%d-> B\n",i,i+1);

	}
	return 0;

}