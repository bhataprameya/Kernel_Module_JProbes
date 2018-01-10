#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<time.h>
#define size 40
typedef struct processes {
	unsigned long address[size];
	long nsec[size];
	time_t sec[size];
	
}process_data;

process_data data;
int main()
{
	int fd,bytes_read;
	int i;
	if((fd = open("/proc/pagefault", O_RDONLY))<0)   //Open the character Device
	{
		printf("Unable to open the character device\n");
		exit(1);
	}
	if((bytes_read = read(fd, &data, sizeof(process_data)))<0)   //Read data from character device
	{
		printf("Unable to read from the  character device\n");
		exit(1);
	}

	printf("The data read are :\n");
	for(i=0;i<size;i++)
		if(data.address[i]!=0 && data.sec[i]!=0 && data.nsec[i]!=0)
		printf("Page Fault at Virtual Address= 0x%lX at time=%ld.%ld \n",data.address[i],data.sec[i],data.nsec[i]);
	close(fd);
}
