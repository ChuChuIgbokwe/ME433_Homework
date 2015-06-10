#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"

#define MAX_STR 255
#define DATA_POINTS 100

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	wprintf(L"Enter row and message (ex: 5Hello!)");
	scanf("%s", &buf[1]);
	buf[0] = 0x80;
	res = hid_write(handle,buf,65);

	int accel_count = 0;
	short x_accel[DATA_POINTS];
	short y_accel[DATA_POINTS];
	short z_accel[DATA_POINTS];
	while (accel_count < DATA_POINTS) 
	{
		buf[0] = 0x81;
		res = hid_write(handle,buf,65);
		res = hid_read(handle,buf,65);

		if (buf[1] == 0)
		{
			x_accel[accel_count] = (( (short) buf[3]) << 8) + (short) buf[2];
			y_accel[accel_count] = (( (short) buf[5]) << 8) + (short) buf[4];
			z_accel[accel_count] = (( (short) buf[7]) << 8) + (short) buf[6];
			for (int i = 0; i < 4; i++)
				wprintf(L"x accel: %d\n",x_accel[accel_count]);
			accel_count++;
		}
	}

	FILE *ofp;
	ofp = fopen("accels.txt","w");
	fprintf(ofp,"x accel:     y_accel:     z_accel:\r\n");
	for (int i=0; i<DATA_POINTS; i++) {
		fprintf(ofp,"%d %d %d\r\n",x_accel[i],y_accel[i],z_accel[i]);
	}
	fclose(ofp);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
