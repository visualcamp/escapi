/* "simplest", example of simply enumerating the available devices with ESCAPI */
#include <stdio.h>
#include "escapi.h"
#include <iostream>
#include <Windows.h>

struct SimpleCapParams capture;

void cb() {
	printf("Streaming!!\n");
}

void main()
{
	int i, j;
	
	/* Initialize ESCAPI */
	
	int devices = setupESCAPI();

	if (devices == 0)
	{
		printf("ESCAPI initialization failure or no devices found.\n");
		return;
	}
	char name[50];
	for (unsigned int i = 0; i < devices; i++) {
		getCaptureDeviceName(i, name, 50);
		printf("%d. camera name is = %s\n", i, name);
		if (strcmp(name, "aGlass DK II (R)") == 0) {
			printf("USB Camera Detected:%d\n", i);
			break;
		}
	}

  /* Set up capture parameters.
   * ESCAPI will scale the data received from the camera 
   * (with point sampling) to whatever values you want. 
   * Typically the native resolution is 320*240.
   */

	
	capture.mWidth = 640;
	capture.mHeight = 480;
	capture.mTargetBuf = new BYTE[640 * 480 * 2];
	
	/* Initialize capture - only one capture may be active per device,
	 * but several devices may be captured at the same time. 
	 *
	 * 0 is the first device.
	 */
	
	if (initCapture(1, &capture) == 0)
	{
		printf("Capture failed - device may already be in use.\n");
		return;
	}
	else {
		startStream(1, std::bind(cb));
	}

	
	/* Go through 10 capture loops so that the camera has
	 * had time to adjust to the lighting conditions and
	 * should give us a sane image..	 
	 */
	SYSTEMTIME st;
	GetSystemTime(&st);
	int frame = 0;
	long startTime = st.wSecond * 1000 + st.wMilliseconds;
	printf("Before Captrue : %d\n", startTime);
	for (i = 0; i < 500; i++)
	{
			
		while (isCaptureDone(1) == 0)
		{
			/* Wait until capture is done.
			 * Warning: if capture init failed, or if the capture
			 * simply fails (i.e, user unplugs the web camera), this
			 * will be an infinite loop.
			 */		   
		}
		printf("Capture Done!\n");
		frame++;
		GetSystemTime(&st);
		long curTime = st.wSecond * 1000 + st.wMilliseconds;
		if (curTime - startTime >= 1000) {
			printf("FPS : %3.1f\n", (float)frame / (curTime - startTime) * 1000);
			startTime = curTime;
			frame = 0;
		}
	}
	
	/* now we have the data.. what shall we do with it? let's 
	 * render it in ASCII.. (using 3 top bits of green as the value)
	 */
	char light[] = " .,-o+O0@";
	for (i = 0; i < 18; i++)
	{
		for (j = 0; j < 24; j++)
		{
			printf("[%d], ", capture.mTargetBuf[i * 24 + j]);
		}
		printf("\n");
	}
	
	deinitCapture(1);	
	int a;
	std::cin >> a;
}