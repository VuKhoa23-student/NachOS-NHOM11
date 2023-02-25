#include "syscall.h"

void InputArray(int a[], int* n)
{
	int i;
	*n = ReadInt();
	for(i = 0; i < *n; i++)
	{
		a[i] = ReadInt();
	}
}

void OutputArray(int a[], int n)
{
	int i;
	WriteChar('\n');
	for(i = 0; i < n; i++)
	{
		WriteInt(a[i]);
		WriteChar(' ');
	}
}

void BubbleSort(int arr[], int size)
{
	int i, j, temp;
    	for (i = 0; i < size - 1; i++) 
	{
        	for (j = 0; j < size - i - 1; j++) 
		{
            		if (arr[j] > arr[j + 1]) 
			{
                	// Swapping elements using a temporary variable
                	temp = arr[j];
                	arr[j] = arr[j + 1];
                	arr[j + 1] = temp;
            		}
        	}
    	}
}

int main()
{
	//int a[100];
	//int n;
	//InputArray(a, &n);
	//OutputArray(a, n);
	//BubbleSort(a, n);
	//OutputArray(a, n);

	//char c;
	//c = ReadChar();
	//WriteChar(c);

	//WriteChar('\n');
	//WriteString("Vu Anh Khoa   21127631");
	//WriteChar('\n');
	//WriteString("Do Minh Triet 21127188");
	//WriteChar('\n');
	//WriteString("Ho Huu Tam    21127421");
	//WriteChar('\n');
	//WriteString("    HDH NHOM 11");
	char buffer[255];
	int id;
	id = Open("hi", 0);
	Seek(2, id);
	Read(buffer, 100, id);
	Close(id);
	return 0;

}
