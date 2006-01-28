/* Helper app for BAFIRC installation on Windows */

#if !defined(WIN32) && !defined(WIN64)
#error "Why do you want to compile if you aren't on Windows?"
#endif

#include <windows.h>
#include <stdio.h>

const char *path_to_root = ".";

#define UNUSED(x) (void)x

int fileexists(char *filename)
{
	FILE *test = fopen(filename, "r");
	if(test)
	{
		fclose(test);
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char *mingdir, *sysdir, *tmp, *tmp2, *tmp3;
	DWORD have_mingdir, have_sysdir;
	char answer;
	int install_pth = 0;
	mingdir = (char *)malloc(512);
	sysdir = (char *)malloc(512);
	FILE *mp;

	printf("BAFSoft BAFIRC\n^^^^^^^^^^^^^^\n");
	printf("Welcome to BAFIRC for Windows! Instead of only using makefile, we use an\n");
	printf("interactive wizard  that sets up the makefile for windows. Lets get started!\n\n");
	printf("Basically, just answer each question. If it is a yes/no question, answer y or n\n");
	printf("If it is multiple choice, use the number of the right response. After you typed\n");
	printf("in your answer, hit enter.\n\n");

	printf("Do you have MinGW? ");
	scanf("%c", &answer);
	if(answer == 'y' || answer == 'Y')
	{
		printf("\nChecking to see if MINGDIR is set...\t\t");
		memset(mingdir, '\0', 512);
		have_mingdir = GetEnvironmentVariable("MINGDIR", (LPTSTR)mingdir, 512);
		if(have_mingdir)
			printf("Found it [%s]\n", mingdir);
		else
		{
			printf("Didn't find it\n");
			printf("Where is MingW installed? IE: C:\\mingw -> ");
			scanf("%s", mingdir);
			printf("\n");
		}
		printf("Checking for pthreads...\t\t\t");
		tmp = (char *)malloc(512);
		tmp2 = (char *)malloc(512);
		sprintf(tmp, "%s\\include\\pthread.h", mingdir);
		sprintf(tmp2, "%s\\lib\\libpthreadGC.a", mingdir);
		if(fileexists(tmp) && fileexists(tmp2))
		{
			printf("Found it\n");
		}
		else
		{
			printf("Didn't find it\n");
			printf("Installing pthreads for windows...\n");
			tmp3 = (char *)malloc(512);
			sprintf(tmp3, "copy %s\\win32\\pthreads\\include\\*.h %s\\include\\", path_to_root, mingdir);
			printf("\t[%s]\n", tmp3);
			system(tmp3);
			sprintf(tmp3, "copy %s\\win32\\pthreads\\lib\\libpthreadGC.a %s\\lib\\", path_to_root, mingdir);
			printf("\t[%s]\n", tmp3);
			system(tmp3);
			sprintf(tmp3, "copy %s\\win32\\pthreads\\lib\\pthreadGC.dll %s\\", path_to_root, sysdir);
			printf("Installed pthreads for windows.\n");
			install_pth = 1;
		}
		printf("Fixing makefiles...\n");
		mp = fopen("./makefile.plat", "w+");
		fprintf(mp, "EXE = .exe\nIDIR = %s\\include\nLDIR = %s\\lib\n", mingdir, mingdir);
		if(install_pth)
			fprintf(mp, "INSTALL_PTHREADS = 1\n");
		fclose(mp);
		system("cd ..");
		printf("\nOk! Good to go! Now type make.\n");
	}
	else
	{
		printf("No makefile system or wizard set up for anything but MingW so far.\n");
	}
	if(mingdir)
		free(mingdir);
	if(sysdir)
		free(sysdir);
	if(tmp)
		free(tmp);
	if(tmp2)
		free(tmp2);
	if(tmp3)
		free(tmp3);
}
