#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	char const_str[]="Not changed";
	char *str;
	long int i = 0;

	str = (char*)malloc(20*sizeof(char));
	if (str==NULL)
	{
		fprintf(stderr, "malloc failed!\n");
		return 0;
	}
	memcpy(str,"Not changed",sizeof(const_str));
	while(1) {
	    printf("[%ld] %s (%p)\n",i,str,(void*)str);
	    sleep(1);
	    i++;
	}
	return 0;
}