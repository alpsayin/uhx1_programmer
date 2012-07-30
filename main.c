/*
 * main.c
 *
 *  Created on: Jul 6, 2012
 *      Author: alpsayin
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termio.h>
#include <time.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdint.h>

#include "main.h"
#include "devtag-allinone.h"
#include "lock.h"
#include "printAsciiHex.h"
#include "serial.h"
#include "uhx1_commander.h"
#include "util.h"
#include "datacollection.h"

int showMainMenu()
{
	int select;
	printf("[1] - Read Mode\n");
	printf("[2] - Set Mode\n");
	printf("[3] - Set Default Channel\n");
	printf("[4] - Set Default Power\n");
	printf("[5] - Jump to Channel (volatile)\n");
	printf("[6] - Select Tx Power (volatile)\n");
	printf("[7] - Read RSSI\n");
	printf("[8] - Dump Eeprom Contens\n");
	printf("[9] - Exit (or Ctrl-C)\n");
	do
	{
		printf("Enter a selection [1-9]");
		scanf("%d", &select);
	}while(select<1 || select>9);
	return select;
}
int showSetModeMenu()
{
	int select;
	printf("[1] - Parallel\n");
	printf("[2] - Serial\n");
	do
	{
		printf("Enter a selection [1-2]");
		scanf("%d", &select);
	}while(select<1 || select>2);
	return select;
}
int showSetDefaultChannelMenu()
{
	int select;
	do
	{
		printf("Enter default channel [0-127]");
		scanf("%d", &select);
	}while(select<0 || select>127);
	return select;
}
int showSetDefaultPowerMenu()
{
	int select;
	do
	{
		printf("Enter default power [3-30]");
		scanf("%d", &select);
	}while(select<3 || select>30);
	return select;
}
int showJumpToChannelMenu()
{
	int select;
	do
	{
		printf("Enter a channel to jump [0-127]");
		scanf("%d", &select);
	}while(select<0 || select>127);
	return select;
}
int showJumpPowerMenu()
{
	int select;
	do
	{
		printf("Enter volatile tx power [3-30]");
		scanf("%d", &select);
	}while(select<3 || select>30);
	return select;
}
int repeatOrContinue()
{
	int retVal;
	printf("Press enter to repeat or press space then enter to get to main menu");
	while (retVal != '\r' && retVal != '\n' && retVal!=' ')
	{
		retVal = getchar();
	}
	if(retVal!=' ')
		return 1;
	else
		return 0;
}
void pressEnterToContinue()
{
	int retVal;
	printf("Press enter to continue");
	while (retVal != '\r' && retVal != '\n')
	{
		retVal = getchar();
		retVal = getchar();
	}
}
void safe_exit(int returnVal)
{
	printf("\nBye bye...\n");
	serial_closeSerialPort();
	closeLogFile();
	exit(returnVal);
}
int main(int argc, char* argv[])
{
	char device[64];
	char logBuffer[64];
	int retVal=0, action=0, repeated=0;
	int interactive=1;

	if(argc==2)
	{
		interactive=0;
	}
	else if(argc==3)
	{
		interactive=1;
	}
	else
	{
		usage();
		exit(0);
	}

	strcpy(device, argv[1]);
	serial_openSerialPort(device, B115200, 50000, 50000);
	openLogFile(UHX1_PROGRAMMER_LOGFILE);

	signal(SIGINT, safe_exit);

	if(interactive)
	{
		if(!strcasecmp(argv[2], "rssi"))
		{						
			retVal = uhx1_readRSSI();
			printf("RSSI = %dd = 0x%02X\n", retVal, retVal);
			sprintf(logBuffer, "%d", retVal);
			registerEvent("RSSI", logBuffer);
			safe_exit(0);
		}
		else if(!strcasecmp(argv[2], "dump"))
		{
			printf("%s", uhx1_dumpMemory());
			safe_exit(0);
		}
		else if(!strcasecmp(argv[2], "mode"))
		{
			printf("Mode = %s\n", uhx1_readMode());
			safe_exit(0);
		}
	}
	else
	{
		do
		{
			repeated=0;
			action=showMainMenu();
			switch(action)
			{
				case 1:
					printf("Mode = %s\n", uhx1_readMode());
					break;
				case 2:
					retVal = showSetModeMenu();
					if(retVal == 1)
						printf("Response = %s\n", uhx1_setMode(COMMAND_PARALLEL));
					else if(retVal == 2)
						printf("Response = %s\n", uhx1_setMode(COMMAND_LOCKSERIAL));
					else
						printf("showSetModeMenu - shouldn't happen\n");
					break;
				case 3:
					retVal = showSetDefaultChannelMenu();
					printf("Response = %s\n", uhx1_setDefCh(retVal));
					break;
				case 4:
					retVal = showSetDefaultPowerMenu();
					printf("Response = %s\n", uhx1_setDefPow(retVal));
					break;
				case 5:
					retVal = showJumpToChannelMenu();
					uhx1_setCh(retVal);
					printf("No response expected\n");
					break;
				case 6:
					retVal = showJumpPowerMenu();
					uhx1_setPow(retVal);
					printf("No response expected\n");
					break;
				case 7:
					do
					{
						retVal = uhx1_readRSSI();
						printf("RSSI = %dd = 0x%02X\n", retVal, retVal);
						sprintf(logBuffer, "%d", retVal);
						registerEvent("RSSI", logBuffer);
						if(!repeated)
							getchar();
						repeated=1;
					}while(repeatOrContinue());
					break;
				case 8:
					printf("%s", uhx1_dumpMemory());
					printf("col 84, row 4 -> Default Tx Power\n");
					printf("col 8A, row 4 -> Default Channel\n");
					break;
				case 9:
					safe_exit(0);
					break;
				default:
					printf("wrong selection");
					break;
			}

			if(!repeated)
				pressEnterToContinue();

		}while(action<9);
	}

	safe_exit(0);

	return 0;
}
