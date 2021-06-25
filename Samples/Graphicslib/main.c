#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>

#include "../../fontloader.h" //Include fontloader.h

PSP_MODULE_INFO("Font Loader Test", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}


/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}


/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int main(void)
{
	SetupCallbacks();
	pspDebugScreenInit();
        initGraphics();

        // Create a (graphics lib) image 'myImage', 250x100
        Image* myImage = createImage(250, 100);
        
        // Load a font
        Font* myFont = Font_Load("./Fonts/REN_STIM.TTF");

	// Set the size of 'myFont' to 60 (pixels)
        Font_SetSize(myFont, 60);
        
        // Print "1st Font" to 'myImage' using 'myFont' at x = 0, y = 50, colored red
        Font_Print_Image(myFont, 0, 50, "1st Font", GU_RGBA(255, 0, 0, 255), myImage);

	// Load a font
        Font* othFont = Font_Load("./Fonts/VERA.TTF");
        
        //Set size of 'othFont' to 30 (pixels)
	Font_SetSize(othFont, 30);

        while(1)
                {
                // Clear screen (black)
                clearScreen(0);
                
                // Blit 'myImage' to the screen (contains the text "1st Font")
                blitAlphaImageToScreen(0, 0, 200, 100, myImage, 50, 50);
                
                // Print "2nd Font" direct to the screen using 'othFont' at x = 230, y = 200, colored white
	        Font_Print_Screen(othFont, 230, 200, "2nd Font", GU_RGBA(255, 255, 255, 255));

                sceDisplayWaitVblankStart();
                flipScreen();
                }

        Font_Unload(myFont);
        Font_Unload(othFont);

	return 0;
}
