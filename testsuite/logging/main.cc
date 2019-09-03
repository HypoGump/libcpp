#include <stdio.h>
#include <libgen.h>

int main(int argc, char* argv[])
{
	printf("%s basename:%s\n", argv[0], ::basename(argv[0]));
}
