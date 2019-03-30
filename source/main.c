#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <switch.h>

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

#include <errno.h>

void	enableTwili()
{
	// Save stock_hbl.nsp
	if (rename("sdmc:/atmosphere/hbl.nsp", "sdmc:/atmosphere/stock_hbl.nsp") == -1) {
		perror("rename");
		return ;
	}

	// install twili hbl
	if (isFileExist("sdmc:/atmosphere/twili_hbl.nsp") == true) {
		if (rename("sdmc:/atmosphere/twili_hbl.nsp", "sdmc:/atmosphere/hbl.nsp") == -1) {
			perror("rename");
			return ;
		}
	} else {
		if (rename("romfs:/twili_hbl.nsp", "sdmc:/atmosphere/hbl.nsp") == -1) {
			perror("rename");
			return ;
		}
	}

	// Rename disable_boot2.flag in boot2.flag
	if (isFileExist("sdmc:/atmosphere/titles/0100000000006480/flags/disable_boot2.flag") == true) {
		rename("sdmc:/atmosphere/titles/0100000000006480/flags/disable_boot2.flag",
				"sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag");
	}

}

void	disableTwili()
{
	if (isFileExist("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag") == true) {
		rename("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag",
				"sdmc:/atmosphere/titles/0100000000006480/flags/disable_boot2.flag");
	}

	rename("sdmc:/atmosphere/hbl.nsp", "sdmc:/atmosphere/twili_hbl.nsp");

	if (isFileExist("sdmc:/atmosphere/stock_hbl.nsp") == true) {
		rename("sdmc:/atmosphere/stock_hbl.nsp", "sdmc:/atmosphere/hbl.nsp");
	} else {
		printf("Error\n");
	}

}

int main(void)
{
	consoleInit(NULL);
	romfsInit();

	while (appletMainLoop()) {
		consoleClear();
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		// Print Header
		printf("================================================================================");
		printf("%s%47s%s", CONSOLE_YELLOW, "Twili Disabler\n", CONSOLE_RESET);
		printf("================================================================================");

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
	romfsExit();

	return (0);
}
