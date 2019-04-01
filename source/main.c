#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <switch.h>

void	printHeader(void)
{
	printf("\x1b[0;0H");
	printf("================================================================================");
	printf("%s%47s%s", CONSOLE_YELLOW, "Twili Disabler\n", CONSOLE_RESET);
	printf("================================================================================");
}


bool	isFileExist(const char *file)
{
	struct stat	st = {0};

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

bool	getTwiliState(void)
{
	return (isFileExist("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag"));
}

void	drawButton(char *string, char *color)
{
	printf("\x1b[20;0H");

	printf("%31s ________________\n", "");
	printf("%31s|                |\n", "");
	printf("%31s|                |\n", "");
	printf("%31s|%s%16s%s|\n", "", color, string, CONSOLE_RESET);
	printf("%31s|                |\n", "");
	printf("%31s|                |\n", "");
	printf("%31s ----------------\n", "");
}

void	warningMessage(char *str)
{
	int	align = 0;

	// align to vertical center
	if (strlen(str) < 80) {
		align = 40 - (strlen(str) / 2);
	}

	consoleClear();
	printHeader();

	printf("\x1b[25;%dH", align);

	printf("%s%s%s", CONSOLE_RED, str, CONSOLE_RESET);

	consoleUpdate(NULL);

	sleep(3);
}

void	enableTwili()
{
	/*// Save stock_hbl.nsp*/
	/*if (rename("sdmc:/atmosphere/hbl.nsp", "sdmc:/atmosphere/stock_hbl.nsp") == -1) {*/
		/*perror("rename");*/
		/*return ;*/
	/*}*/

	/*// install twili hbl*/
	/*if (isFileExist("sdmc:/atmosphere/twili_hbl.nsp") == true) {*/
		;/*if (rename("sdmc:/atmosphere/twili_hbl.nsp", "sdmc:/atmosphere/hbl.nsp") == -1) {*/
			/*perror("rename");*/
			/*return ;*/
		/*}*/
	/*} else {*/
		/*if (rename("romfs:/twili_hbl.nsp", "sdmc:/atmosphere/hbl.nsp") == -1) {*/
			/*perror("rename");*/
			/*return ;*/
		/*}*/
	/*}*/

	// Rename disable_boot2.flag in boot2.flag
	if (isFileExist("sdmc:/switch/twili_disabler/boot2.flag") == true) {
		rename("sdmc:/switch/twili_disabler/boot2.flag",
				"sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag");
	}
	
	/*if (isFileExist("sdmc:/switch/twili_disabler/a") == true) {*/
		/*if (remove("sdmc:/switch/twili_disabler/b") == -1) {*/
			/*perror("remove");*/
		/*} else if (rename("sdmc:/switch/twili_disabler/a",*/
				/*"sdmc:/switch/twili_disabler/b") == -1) {*/
			/*perror("rename");*/
		/*}*/
	/*}*/

}

void	disableTwili()
{
	// if do not have stock_hbl, abort
	if (isFileExist("sdmc:/switch/twili_disabler/stock_hbl.nsp") == false) {
		warningMessage("Error : Please put stock_hbl.nsp in /switch/twili_disabler");
		return ;
	}

	// backup boot2.flag
	if (isFileExist("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag") == true) {
		if (rename("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag",
				"sdmc:/switch/twili_disabler/boot2.flag") == -1) {
			perror("rename()");
		}
	}

	// save twili_hbl.nsp
	if (isFileExist("sdmc:/switch/twili_disabler/twili_hbl.nsp") == true) {
		remove("sdmc:/atmosphere/hbl.nsp");
	} else {
		rename("sdmc:/atmosphere/hbl.nsp", "sdmc:/switch/twili_disabler/twili_hbl.nsp");
	}

	// Install stock_hbl
	rename("sdmc:/switch/twili_disabler/stock_hbl.nsp", "sdmc:/atmosphere/hbl.nsp");

}

bool	checkNeededFile()
{
	bool	err = false;
	char	*files[] = {
		"sdmc:/switch/twili_disabler",
		"sdmc:/switch/twili_disabler/boot2.flag",
		"sdmc:/switch/twili_disabler/twili_hbl.nsp",
		"sdmc:/switch/twili_disabler/stock_hbl.nsp",
		NULL
	};

	printf("Check requiered files :\n\n");

	for (int i = 0; files[i]; i++) {
		if (isFileExist(files[i]) == false) {
			printf("%s [%sKO%s]\n", files[i], CONSOLE_RED, CONSOLE_RESET);
			err = true;
		} else {
			printf("%s [%sOK%s]\n", files[i], CONSOLE_GREEN, CONSOLE_RESET);
		}
	}

	return (err);
}

int main(void)
{
	u64	kDown = 0;

	consoleInit(NULL);

	// Check if file are here
	if (checkNeededFile() == true) {
		while (1) {
		hidScanInput();
			kDown = hidKeysDown(CONTROLLER_P1_AUTO);

			printf("%s\x1b[25;15H" "Please put missing files in /switch/twili_disabler/", CONSOLE_YELLOW);
			printf("\x1b[27;35H" "Press + to quit\n%s", CONSOLE_RESET);

			if (kDown & KEY_PLUS) {
				consoleExit(NULL);
				return (-1);
			}
			consoleUpdate(NULL);
		}
	}

	while (appletMainLoop()) {
		consoleClear();
		hidScanInput();
		kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		printHeader();

		if (getTwiliState() == true) {
			drawButton("Twili Enabled !", CONSOLE_GREEN);
			printf("\x1b[50;0H");
			printf("Press A to %sDisable%s Twili", CONSOLE_RED, CONSOLE_RESET);

			if (kDown & KEY_A) {
				disableTwili();
			}
		} else {
			drawButton("Twili Disabled !", CONSOLE_RED);
			printf("\x1b[50;0H");
			printf("Press A to %sEnable%s Twili", CONSOLE_GREEN, CONSOLE_RESET);

			if (kDown & KEY_A) {
				enableTwili();
			}
		}

		if (kDown & KEY_PLUS)
			break;

		consoleUpdate(NULL);
	}

	consoleExit(NULL);

	return (0);
}
