/*
 * main.h
 *
 *  Created on: Jul 6, 2012
 *      Author: alpsayin
 */

#ifndef MAIN_H_
#define MAIN_H_

#define UHX1_PROGRAMMER_LOGFILE "rssi.log"

int showMainMenu(void);
int showSetModeMenu();
int showSetDefaultChannelMenu();
int showSetDefaultPowerMenu();
int showJumpToChannelMenu();
int showJumpPowerMenu();

int repeatOrContinue();
void pressEnterToContinue();
void safe_exit(int returnVal);

int main(int argc, char* argv[]);

#endif /* MAIN_H_ */
