#include <nds.h>
#include <stdio.h>


int main(void)
{
	consoleDemoInit();

  iprintf("Hello World!");

  while(true)
		swiWaitForVBlank();
}
