#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <string.h>
#include <direct.h>		/* _getcwd */
#include <string>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <iostream>
using namespace std;

//*****************************

string dir_mon;		//directory being monitored
string dir_bcp;		//directory containing backup

struct files_details
{
	char name[40];
	long long size;
	char lmt[40]; //last modified time
	char lat[40]; //last access time
};

//******************************

void initialise();
void fileinfo(int);
void comp();
void restre();
void ask();
BOOL CtrlHandler(DWORD);

//******************************

int main()
{
	cout << "Welcome to File Security Software\nLet's start!\n";
	char cCurrentPath[100];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));
	dir_bcp = cCurrentPath;
	FILE *fp;
	if (fp = fopen("dir_mon.txt", "r"))
	{
		rewind(fp);
		char ch = '\0';
		string k = "";
		while (ch != EOF)
		{
			ch = fgetc(fp);
			if (ch != EOF && ch != '\n')
				k = k + ch;
		}
		dir_mon = k;
	}
	else
	{
		string dm = "";
		cout << "\nThis is your first run. Please enter the directory to be monitored... ";
		cin >> dm;
		const char* fd = dm.c_str();
		fp = fopen("dir_mon.txt", "w");
		fprintf(fp, "%s", fd);
		dir_mon = dm;
	}
	fclose(fp);
	initialise();
	return 0;
}

//******************************

void initialise()				//Create backup
{
	system("cls");
	int df;
	cout << "What do you want to do?\n1. View last run details\n2. Start a new monitoring session\n3. Exit\n\nEnter your choice... ";
	cin >> df;
		if (df == 1)
		{
			system("cls");
			FILE *fl;
			char ch = 'a';
			cout << endl << "The following events occured during the last-run\n" << endl;
			string lg = dir_bcp + "\\logs" + "\\log.txt";
			fl = fopen(lg.c_str(), "r");
			while (ch != EOF)
			{
				ch = fgetc(fl);
				cout << ch;
			}
			fclose(fl);
			ask();
		}
		else if (df == 2)
		{
			system("cls");
			char jk;
			cout << "Warning! Starting a new session will delete the details of the last run. Are you sure? (Y/N)";
			cin >> jk;
			if (jk == 'y' || jk == 'Y')
			{
				system("cls");
				string hj = "rd /s /q " + dir_bcp + "\\backup";
				string lj = "rd /s /q " + dir_bcp + "\\logs";
				system(hj.c_str());
				system(lj.c_str());
				cout << ("Checking if processor is available...");
				if (system(NULL)) puts("Ok");
				else exit(EXIT_FAILURE);
				string cr = "dir " + dir_mon;
				cout << ("\nDirectory being monitored...\n");
				system(cr.c_str());
				cout << ("\nCreating backup directory...\n");
				char cCurrentPath[100];
				_getcwd(cCurrentPath, sizeof(cCurrentPath));
				dir_bcp = cCurrentPath;
				string cu = "mkdir " + dir_bcp + "\\backup";
				system(cu.c_str());
				string cw = "robocopy " + dir_mon + " " + dir_bcp + "\\backup" + " /e";
				cout << "\nCreating backup...\n";
				system(cw.c_str());
				fileinfo(1);
			}
			else
			{
				initialise();
			}
		}
		else if (df == 3)
		{
			exit(0);
		}
		else
		{
			cout << "\nInvalid option!. Please enter your choice again... ";
			initialise();
		}
}

void fileinfo(int r)
{
	//Create a file containing the details like last modified time
	FILE *fp, *ds, *ds1;
	char ch = 'a';
	long long recsize;
	string ar, k, dl, dk, tf, bk, lg;
	dk = dir_bcp + "\\logs";
	_mkdir(dk.c_str());
	if (r == 1)								//first-run
	{
		cout << "\nCreating initial record of files present in the directory: " << dir_mon << endl;
		ar = dir_bcp + "\\logs" + "\\initrecord.txt";
		dl = dir_bcp + "\\logs" + "\\initnames.txt";
	}
	else
	{
		ar = dir_bcp + "\\logs" + "\\newrecord.txt";
		dl = dir_bcp + "\\logs" + "\\newnames.txt";
		lg = dir_bcp + "\\logs" + "\\log.txt";
		bk = dir_bcp + "\\logs" + "\\bklog.txt";
		ds1 = fopen(ar.c_str(), "w");
		fclose(ds1);
		ds1 = fopen(dl.c_str(), "w");
		fclose(ds1);
		ds1 = fopen(lg.c_str(), "w");
		fclose(ds1);
		ds1 = fopen(bk.c_str(), "w");
		fclose(ds1);
	}
	tf = "dir " + dir_mon + " /b /a-d-h > " + dl;
	const char* rt = tf.c_str();
	system(rt);
	ds = fopen(dl.c_str(), "r");
	rewind(ds);
	for (;;)
	{
		k = "";
		for (;;)
		{
			ch = fgetc(ds);
			if (ch == EOF)
				break;
			if (ch == '\n')
				break;
			k = k + ch;
		}
		if (ch == EOF)
			break;
		string fn = dir_mon + "\\" + k;
		struct stat st = { 0 };
		struct stat b;
		const char* h = fn.c_str();
		struct files_details fd;
		recsize = sizeof(fd);
		if (!stat(h, &b))
		{
			strftime(fd.lmt, 40, "%d/%m/%Y %H:%M:%S", localtime(&b.st_mtime));
			strftime(fd.lat, 40, "%d/%m/%Y %H:%M:%S", localtime(&b.st_atime));
			strcpy(fd.name, k.c_str());
			fd.size = (long long)b.st_size;
		}
		else
		{
			printf("Error getting times/ details!\n");
		}
		fp = fopen(ar.c_str(), "a+");
		fseek(fp, 0, SEEK_END);
		fwrite(&fd, recsize, 1, fp);
		fclose(fp); 
	}
	fclose(ds);
	if (r == 1)
	{
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
		{
			system("cls");
			cout << endl << "Monitoring started successfully\nPlease close the program window to stop monitoring further...";
			while (1) {}
		}
		else
		{
			system("cls");
			printf("\nERROR: Monitoring could not be started. The program will now terminate.");
			exit(0);
		}
	}
}
void comp()
{
	FILE *fp, *ds, *fq, *fl, *ds1;
	string t1="", t2="", ntf="";
	long long recsize=0;
	char ch;
	struct files_details fd;
	recsize = sizeof(fd);
	string in = dir_bcp + "\\logs" + "\\initnames.txt";
	string ir = dir_bcp + "\\logs" + "\\initrecord.txt";
	string nr = dir_bcp + "\\logs" + "\\newrecord.txt";
	ds = fopen(in.c_str(), "r");
	rewind(ds);
	for (;;)
	{
		string k = "";
		for (;;)
		{
			ch = fgetc(ds);
			if (ch == EOF)
				return;
			if (ch == '\n')
				break;
			k = k + ch;
		}
		fp = fopen(ir.c_str(), "r");				//initrecord
		rewind(fp);
		while (fread(&fd, recsize, 1, fp) == 1)
		{
			if (strcmp(fd.name, k.c_str()) == 0)
			{
				t1 = fd.lmt;
				break;
			}
		}
		fclose(fp);
		fq = fopen(nr.c_str(), "r");				//newrecord
		rewind(fq);
		int flag = 0;
		while (fread(&fd, recsize, 1, fq) == 1)
		{
			if (strcmp(fd.name, k.c_str()) == 0)
			{
				t2 = fd.lmt;
				flag = 1;
				break;
			}
		}
		fclose(fq);
		string bk = dir_bcp + "\\logs" + "\\bklog.txt";
		string lg = dir_bcp + "\\logs" + "\\log.txt";		
		if (flag == 0)
			ntf = k+ " -->File deleted" +"\n";
		else
		{
			if (t1 == t2)
				continue;
			else
				ntf = k + " --> File modified at " + t2 + "\n";
		}
		const char* asd = ntf.c_str();
		cout << asd;
		fl = fopen(lg.c_str(), "a");
		fprintf(fl,"%s",asd);
		fclose(fl);
		ds1 = fopen(bk.c_str(), "a");
		fprintf(ds1, "%s", k.c_str());
		char* er = "\n";
		fprintf(ds1, "%s", er);
		fclose(ds1);
	}
	fclose(ds);
}

void ask()
{
	int i;
	cout << endl << "\nPlease select an option:\n1. Restore all modified/deleted files\n2. Restore individual file\n3. Go back to main menu\nEnter your choice...";
	for (;;)
	{
		cin >> i;
		if (i == 1)
		{
			string cw = "robocopy " + dir_bcp + "\\backup" + " " + dir_mon;
			cout << "\nRestoring all files...\n";
			if (system(cw.c_str())) puts("Successful\n");
			else puts("Failed");
			break;
		}
		else if (i == 2)
		{
			restre();
			break;
		}
		else if (i == 3)
		{
			initialise();
			break;
		}
		else
		{
			cout << "\nInvalid choice! Please enter your choice again...";
			continue;
		}
	}
	initialise();
}
void restre()
{
	system("cls");
	cout << endl << "Please enter the file name you want to restore\n";
	string nm;
	cin >> nm;
	struct stat st = { 0 };
	struct stat b;
	string fd = dir_bcp + "\\backup\\" + nm;
	const char* h = fd.c_str();
	if (!stat(h, &b))
	{
		string cw = "robocopy " + dir_bcp + "\\backup " + dir_mon + " " + nm;
		cout << "\nRestoring file " << nm << "...";
		system(cw.c_str());
		cout << "\nFile " << nm << " Restored successfully\nWant to restore another file? (Y/N)\n";
		char op;
		cin >> op;
		for (;;)
		{
			if (op == 'Y' || op == 'y')
			{
				restre();
				break;
			}
			else if (op == 'n' || op == 'N')
			{
				initialise();
				break;
			}
			else
			{
				cout << endl << "Invalid option. Please enter correct option (Y/N)...";
				cin >> op;
				continue;
			}
		}
	}
	else
	{
		cout << endl << "Error: File not found in the backup.";
		initialise();
	}
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
	{
		fileinfo(0);
		comp();
		Beep(750, 300);
		return(TRUE);
	}

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
	{
		fileinfo(0);
		comp();
		Beep(750, 300);
		return(TRUE);
	}

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
	{
		fileinfo(0);
		comp();
		Beep(750, 300);
		return(TRUE);
	}

	case CTRL_LOGOFF_EVENT:
	{
		fileinfo(0);
		comp();
		Beep(750, 300);
		return(TRUE);
	}

	case CTRL_SHUTDOWN_EVENT:
	{
		fileinfo(0);
		comp();
		Beep(750, 300);
		return(TRUE);
	}
	default:
		return FALSE;
	}
}
