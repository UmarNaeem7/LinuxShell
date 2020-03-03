#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <stack>
#include <stdlib.h>
using namespace std;

stack <char*> dirList;		//stl stack to entertain push and pop commands
string passwords[10000];	//hash table to store passwords
string privateFiles[100];	//store names of private files
int count = 0;			//store count of private files

bool isPrivate(char * fname)	//helper function to determine whether a file is private or not
{	
	string s(fname);
	for (int i=0;i<count;i++)
	{
		if (s == privateFiles[i])
			return true;
	}
	return false;
}

void runPWD() //function for pwd command
{
	char dir[1000];
	getcwd(dir, sizeof(dir));
	cout<<dir<<endl;
}

void runLS() //function for ls
{
	char* buff = new char[256];
	char* dptr = new char[1000];
	buff = getcwd(NULL, 0);
	DIR* ptr = opendir(buff);
	struct dirent* c;
	if (ptr)
	{
		while ((c = readdir(ptr)) != NULL)
		{
			dptr = c->d_name;
			if (isPrivate(dptr) == false)  //only view a file if its not private
			{
				if (c->d_type != DT_DIR)
					cout<<dptr<<endl;
				else if (c->d_type == DT_DIR && strcmp(dptr,".") != 0 && strcmp(dptr,"..") != 0)
					cout<<dptr<<endl;
			}
		}
	}
	closedir(ptr);
}

void privateLS() //function for private ls
{
	char* buff = new char[256];
	char* dirr = new char[1000];
	buff = getcwd(NULL, 0);
	DIR* dir = opendir(buff);
	struct dirent* c;
	if (dir)
	{
		while ((c = readdir(dir)) != NULL)
		{
			dirr = c->d_name;
			if (isPrivate(dirr) == false)  //if a file is not private then simply show it on console
			{
				if (c->d_type != DT_DIR)
					cout<<dirr<<endl;
				else if (c->d_type == DT_DIR && strcmp(dirr,".") != 0 && strcmp(dirr,"..") != 0)
					cout<<dirr<<endl;
			}
			else	//if file is private, then ask user for password and determine its validity
			{
				int hash = 0;
				int i = 0;
				while (dirr[i] != '\0')
				{
					hash += int(dirr[i]);
					i++;
				}
				cout<<"\nPassword : ";
				string pass;
				bool flag = false;
				getline(cin, pass);
				if (pass == passwords[hash])
					cout<<dirr<<endl;
				else
				{
					string choice = "y";
					cout<<"Incorrect password!"<<endl;
					while (choice == "y" && flag == false)
					{
						cout<<"Do you wish to retry? (y/n) ";
						getline(cin, choice);
						if (choice == "y")
						{	
							string p;
							cout<<"Password : ";
							getline(cin, p);
							if (p == passwords[hash])
							{
								cout<<dirr<<endl;
								flag = true;
							}
							else
								cout<<"Incorrect password!"<<endl;
						}
					}
				}
			}
		}
	}
	closedir(dir);
}

void changeDIR(char* path) //function for cd command
{	
	//cout<<path<<endl;
	char p[100];
	int i = 3, j = 0;
	while (path[i] != '\0')
		p[j++] = path[i++];
	p[j] = '\0';
	//cout<<p<<endl;
	if (chdir(p) == -1)
		cout<<"Directory not found!!"<<endl;
}

void pushDir(char* path)	//function for push command
{	
	char *p = new char[100];
	int i = 5, j = 0;
	while (path[i] != '\0')
		p[j++] = path[i++];
	p[j] = '\0';
	if (chdir(p) == -1)
	{
		cout<<"Directory not found!!"<<endl;
		return;
	}
	if (strcmp(path, "..") == true)
	{
		char* npath = new char[100];
		npath = getcwd(NULL, NULL);
		dirList.push(npath);
		return;
	}
	dirList.push(p);
}

void popDir()		//function for pop command
{
	if (dirList.empty())
	{
		cout<<"ERROR: No directory has been pushed before!"<<endl;
		return;	
	}
	char *p = new char[100];
	strcpy(p, dirList.top());
	dirList.pop();
	if (chdir(p) == -1)
	{
		cout<<"Directory not found!!"<<endl;
		return;
	}
}

void showTop()		//function for pop -showtop command
{
	if (dirList.empty())
	{
		cout<<"ERROR: No directory has been pushed before!"<<endl;
		return;	
	}
	cout<<dirList.top()<<endl;
}

void printStack()	//function to show contents of stack
{
	while (dirList.empty() == false)
	{
		cout<<dirList.top()<<endl;
		dirList.pop();
	}
}

void makePrivate(char* filename)	//function to implement private command
{
	char *p = new char[100];
	int i = 8, j = 0;
	while (filename[i] != '\0')
		p[j++] = filename[i++];
	p[j] = '\0';
	if (access(p, F_OK) != -1) //file exists
	{
		int hash = 0;
		i = 0;
		while (p[i] != '\0')
		{
			hash += int(p[i]);
			i++;
		}
		//cout<<"hash = "<<hash<<endl;
		if (hash >= 10000)	
		{
			cout<<"Hash table size insufficient!"<<endl;
			return;
		}
		string pass;
		cout<<"password : ";
		getline(cin, pass);	//all lengths allowed in password
		passwords[hash] = pass;
		string s(p);
		privateFiles[count++] = p;
		cout<<p<<" secured!"<<endl;
	}
}

void tokenizePath()	//helper to tokenize path
{
	char dir[1000];
	getcwd(dir, sizeof(dir));
	char* t = strtok(dir, "/");
	char* s = new char[50];
	while (t != NULL){
		strcpy(s, t);
		t = strtok(NULL, "/");
	}
	cout<<s<<"> ";
}

int main()
{	
	string roll = "umar - ";
	while (true)
	{	
		cout<<roll;
		tokenizePath();
		string command;
		getline(cin, command);
		if (command == "ls")
		{
			runLS();
		}
		else if (command == "pwd")
		{
			runPWD();
		}
		else if (command.find("cd") == 0)
		{
			char* path = &command[0u];
			changeDIR(path);
		}
		else if (command.find("push") == 0)
		{
			char* path = &command[0u];
			pushDir(path);
		}
		else if (command == "pop")
		{
			char* path = &command[0u];
			popDir();
		}
		else if (command == "pop -showtop")
		{
			showTop();
		}
		else if (command.find("private") == 0)
		{
			char* file = &command[0u];
			makePrivate(file);
		}
		else if (command == "ls -private")
		{
			privateLS();
		}
		else if (command == "exit")
		{
			return 1;
		}
		else if (command == "cls" || command == "clear")
		{
			system("clear");
		}
		else 
		{
			cout<<"ERROR: Command not recognized!!"<<endl;
		}
	}
	return 0;
}
