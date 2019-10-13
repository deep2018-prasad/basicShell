#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int MAX_USR_CHAR = 10000;
char historyFile[] = ".basic_shell_history";
char homeDir[] = "/home/";
char * historyPath;

char * str_concatanate(char * str1, char * str2)
{
	int len;
	char * str3;
	
	int i, j;
	i = 0;
	while (str1[i] != '\0')
	{
		i++;
	}
	
	len = len + i;
	i = 0;
	
	while (str2[i] != '\0')
	{
		i++;
	}
	len = len + i;
	
	str3 = malloc (sizeof (char) * len);
	
	i = 0;
	while (str1 [i] != '\0')
	{
		str3[i] = str1[i];
		i++;
	}
	j = 0;
	while (str2[j] != '\0')
	{
		str3[i] = str2[j];
		i++;
		j++;
	}
	str3[i] = '\0';
	return str3;
}

int comparing_string(char *a, char *b)
{
	int i,j, n;
	i = 0; j = 0;
	while (*(a+i) != '\0')
	{
		i++;
	}
	while (*(b+j) != '\0')
	{
		j++;
	}
	
	if(i != j)
	{
		return 0;
	}
	else
	{
		n = i;
		for( i = 0; i<n; i++)
		{
			if(a[i] != b[i])
			{
				return 0;
			}
		}
	}
	return 1;
}

void sigHdlr(int sigAct)
{
	printf ("\n");
}

int main ()
{
	struct  sigaction sa;
	memset (&sa, 0, sizeof (struct sigaction));

	sa.sa_handler = &sigHdlr;
	sigaction (SIGINT, &sa, NULL);

	char * userName = getlogin ();
	char ** argList = NULL;
	char * tempStr = NULL;

	tempStr = str_concatanate (homeDir, userName);
	tempStr = str_concatanate (tempStr, "/");
	historyPath = str_concatanate (tempStr, historyFile);
	if (tempStr != NULL)
	{
		free (tempStr);
	}
	fprintf (stderr, "%s\n", historyPath);
	int pid;
	char ch;
	int i, j;
	int newLineFlag;
	int sizeArgList;
	int prevIndex;
	char hostName[100];
	fprintf (stderr, "\n                                          Basic Shell\nCopyright (C) 2019 version.1.0 <deeprasad1996@gmail.com, ankitgupta.cs40@gmail.com>\n\n");
	system ("date");
	printf ("\n");
	FILE * fp;
	fp = fopen ("/etc/hostname", "r");
	fscanf (fp, "%s", hostName);
	fclose (fp);
	fflush (fp);

	tempStr = NULL;

	while (1)
	{
		fprintf (stderr, "[%s@%s]>> ", userName, hostName);
		sizeArgList = 0;

		i = 0;
		j = 0;
		newLineFlag = 0;

		int argListLength = MAX_USR_CHAR/2;
		argList = malloc (sizeof (char *) * argListLength);

		tempStr = (char *) malloc (sizeof (char) * MAX_USR_CHAR);
		memset (tempStr, 0, MAX_USR_CHAR);

		prevIndex = 0;
		while (1)
		{
			if (i == MAX_USR_CHAR)
			{
				fprintf(stderr, "[Error] Command line Buffer Full(max size. %d).\n", MAX_USR_CHAR);
				break;
			}

			ch = getc(stdin);
			i++;
			
			if (ch == 32 && j > 0) // blank space
			{
				tempStr[j] = '\0';
				j++;
				argList[sizeArgList] = &tempStr[prevIndex];
				prevIndex = j;
				sizeArgList++;
			}
			else if (ch == '\n' && i != 1) //new line character
			{
				if (j > 0)
				{
					tempStr[j] = '\0';
					j++;
					argList[sizeArgList] = &tempStr[prevIndex];
					sizeArgList++;
				}
				break;
			}
			else if (ch == '\n' && i == 1)
			{
				newLineFlag = 1;
				break;
			}
			else if (ch != 32)
			{
				tempStr[j] = ch;
				j++;
			}
		}

		if (newLineFlag == 1)
		{
			continue;
		}

		fp = fopen(historyPath, "a+");
		if (fp != NULL)
		{
			for (i = 0; i < sizeArgList; i++)
			{
				fprintf (fp, "%s ", argList[i]);
			}
			fprintf(fp, "\n");
			fclose (fp);
			fflush (fp);
		}
		else
		{
			fflush (fp);
			fprintf(stderr, "History is not being stored.\n");
		}

		argList[sizeArgList] = NULL;
		sizeArgList++;

		if (comparing_string (argList[0], "exit")) 
		{
			if (tempStr != NULL)
			{
				free(tempStr);
			}

			if (argList != NULL)
			{
				free (argList);
				argList = NULL;
			}
			exit (EXIT_SUCCESS);
		}
		else if (comparing_string (argList[0], "history"))
		{
			
		}

		pid = fork ();

		if (pid == 0)  //child process
		{
			execvp (argList[0], argList);
			perror ("execvp");
			fprintf (stderr, "Error in command [%s]\n", argList[0]);
			exit (EXIT_FAILURE);
		}
		else if (pid < 0) //error case
		{
			fprintf (stderr, "Error in fork.\n");
			perror ("fork");
		}
		else //parent process
		{
			int status;
			wait (&status);

			if (!WIFEXITED(status))
			{
				fprintf (stderr, "Child process not exited.\n");
			}

			if (WIFSIGNALED (status))
			{
				fprintf (stderr, "Child was signalled.\n");
			}

			if (tempStr != NULL)
			{
				free(tempStr);
			}

			if (argList != NULL)
			{
				free (argList);
				argList = NULL;
			}
		}
	}
	if (historyPath != NULL)
	{
		free (historyPath);
	}
	return 0;
}
