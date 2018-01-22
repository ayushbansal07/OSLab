#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define LOOP 100
using namespace std;
int main()
{

	int pipe1[2];
	int pipe2[2];
	int pipe3[2];
	pipe(pipe1);
	pipe(pipe2);
	pipe(pipe3);

	int a[LOOP];
	int a2[LOOP];
	int a3[LOOP];

	int x = fork();
	if(x==0)
	{
		int y = fork();
		if(y==0)
		{
			int z = fork();
			if (z==0)
			{
				//A
				unsigned int seed = 1;
				for(int i=0;i<LOOP;i++)
				{
					a[i] = rand_r(&seed)%101;
				}
				sort(a,a+LOOP);
				cout<<"A:    ";
				for(int i=0;i<LOOP;i++)
				{
					cout<<a[i]<<' ';
				}
				cout<<endl;
				write(pipe1[1],a,LOOP*sizeof(int));
			}
			else
			{
				//B
				unsigned int seed = 2;
				for(int i=0;i<LOOP;i++)
				{
					a2[i] = rand_r(&seed)%101;
				}
				sort(a2,a2+LOOP);
				cout<<"B:    ";
				for(int i=0;i<LOOP;i++)
				{
					cout<<a2[i]<<' ';
				}
				cout<<endl;
				write(pipe2[1],a2,LOOP*sizeof(int));
			}
		}
		else
		{
			//C
			unsigned int seed = 3;
			for(int i=0;i<LOOP;i++)
			{
				a3[i] = rand_r(&seed)%101;
			}
			sort(a3,a3+LOOP);
			cout<<"C:    ";
			for(int i=0;i<LOOP;i++)
			{
				cout<<a3[i]<<' ';
			}
			cout<<endl;
			write(pipe3[1],a3,LOOP*sizeof(int));
		}
	}
	else
	{
		//D
		int b1[LOOP],b2[LOOP],b3[LOOP];
		int b[3*LOOP];
		read(pipe1[0],b1,LOOP*sizeof(int));
		read(pipe2[0],b2,LOOP*sizeof(int));
		read(pipe3[0],b3,LOOP*sizeof(int));

		//Merge: BEGIN
		int i=0,j=0,k=0,t=0;
		while(i<LOOP && j<LOOP && k<LOOP)
		{
			if(b1[i]<b2[j])
			{
				if(b1[i]<b3[k])
				{
					b[t] = b1[i];
					i++;
				}
				else
				{
					b[t] = b3[k];
					k++;
				}
			}
			else
			{
				if(b2[j]<b3[k])
				{
					b[t] = b2[j];
					j++;
				}
				else
				{
					b[t] = b3[k];
					k++;
				}
			}

			t++;
		}
		if(i==LOOP)
		{
			while(j<LOOP && k<LOOP)
			{
				if(b2[j]<b3[k])
				{
					b[t] = b2[j];
					j++;
				}
				else
				{
					b[t] = b3[k];
					k++;
				}
				t++;
			}
			while(j<LOOP)
			{
				b[t] = b2[j];
				j++;
				t++;

			}
			while(k<LOOP)
			{
				b[t] = b3[k];
				k++;
				t++;
			}
		}
		else if(j==LOOP)
		{
			while(i<LOOP && k<LOOP)
			{
				if(b1[i]<b3[k])
				{
					b[t] = b1[i];
					i++;
				}
				else
				{
					b[t] = b3[k];
					k++;
				}
				t++;
			}
			while(i<LOOP)
			{
				b[t] = b1[i];
				i++;
				t++;

			}
			while(k<LOOP)
			{
				b[t] = b3[k];
				k++;
				t++;
			}
		}
		else
		{
			while(j<LOOP && i<LOOP)
			{
				if(b2[j]<b1[i])
				{
					b[t] = b2[j];
					j++;
				}
				else
				{
					b[t] = b1[i];
					i++;
				}
				t++;
			}
			while(j<LOOP)
			{
				b[t] = b2[j];
				j++;
				t++;

			}
			while(i<LOOP)
			{
				b[t] = b1[i];
				i++;
				t++;
			}
		}
		//Merge: END

		cout<<"D:    ";
		for(int i=0;i<3*LOOP;i++)
		{
			cout<<b[i]<<' ';
		}
		cout<<endl;


	}
}