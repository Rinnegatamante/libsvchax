#ifndef LIBKHAX_AS_LIB

#include <3ds.h>
#include <3ds/services/am.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "libsvchax.h"

#define KHAX_lengthof(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))

s32 g_backdoorResult = -1;

s32 dump_chunk_wrapper()
{
	__asm__ volatile("cpsid aif");
	g_backdoorResult = 0x6666abcd;
	return 0;
}

// Test access to "am" service, which we shouldn't have access to, unless khax succeeds.
Result test_am_access_inner()
{
	Handle testHandle;
	Result res = srvGetServiceHandleDirect(&testHandle, "am:u");
	if (testHandle) svcCloseHandle(testHandle);
	return res;
}

// Self-contained test.
void test_am_access_outer(int testNumber)
{
	Result result = test_am_access_inner();
	printf("amtest:0x%X\n", result);
}


int main()
{
	// Initialize services
/*	srvInit();			// mandatory
	aptInit();			// mandatory
	hidInit(NULL);	// input (buttons, screen)*/
	gfxInitDefault();			// graphics
/*	fsInit();
	sdmcInit();
	hbInit();
	qtmInit();*/

	consoleInit(GFX_BOTTOM, NULL);

	consoleClear();

	test_am_access_outer(1); // test before libkhax

	haxInit(); // Performing svchax to grant all services access

	printf("backdoor returned %08lx\n", (svcBackdoor(dump_chunk_wrapper), g_backdoorResult));

	test_am_access_outer(2); // test after libkhax

	printf("khax demo main finished\n");
	printf("Press X to exit\n");


	while (aptMainLoop())
	{
		// Wait next screen refresh
		gspWaitForVBlank();

		// Read which buttons are currently pressed 
		hidScanInput();
		u32 kDown = hidKeysDown();
		(void) kDown;
		u32 kHeld = hidKeysHeld();
		(void) kHeld;

		// If START is pressed, break loop and quit
		if (kDown & KEY_X){
			break;
		}

		//consoleClear();

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	// Exit services
/*	qtmExit();
	hbExit();
	sdmcExit();
	fsExit();*/
	gfxExit();
/*	hidExit();
	aptExit();
	srvExit();*/

	// Return to hbmenu
	return 0;
}

#endif // LIBKHAX_AS_LIB
