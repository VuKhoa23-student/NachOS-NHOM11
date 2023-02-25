//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.



#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "math.h"

#define max_buff 255
#define MaxFileLength 32 // Do dai quy uoc cho file name

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	Cac quy dinh su dung thanh ghi:
//
// 	   1. Thanh ghi r2: Chua ma lenh syscall va ket qua thuc hien o syscall se tra nguoc ve
// 	   2. Thanh ghi r4: Tham so thu nhat
//	   3. Thanh ghi r5: Tham so thu hai
//	   4. Thanh ghi r6: Tham so thu ba	   
//	   5. Thanh ghi r7: Tham so thu tu
//
// 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

// Increase PC 
void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    	counter = machine->ReadRegister(NextPCReg);
    	machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}

// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char* User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;
		
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}


// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}


// Ham xu ly ngoai le runtime Exception va system call
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

	// Bien toan cuc cho lop SynchConsole
	
	
	//Bat dau
	switch (which) {
	case NoException:
		return;

	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;

	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		interrupt->Halt();
		break;

	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		interrupt->Halt();
		break;

	case SyscallException:
		switch (type){

		case SC_Halt:
			// Input: Khong co
			// Output: Thong bao tat may
			// Chuc nang: Tat HDH
			DEBUG('a', "\nShutdown, initiated by user program. ");
			printf("\nShutdown, initiated by user program. ");
			interrupt->Halt();
			return;
		case SC_ReadInt:
			char* buffer;
			int numBytes;
			int res;
			bool soAm;
			int index;
			bool is_valid;
			soAm = false;
			res = 0;
			index = 0;
			buffer = new char[max_buff + 1];
			numBytes  = gSynchConsole->Read(buffer, max_buff);
			if(buffer[0] == '\0')
			{
				machine->WriteRegister(2, res);
				delete buffer;
				break;
			}
			// check if input integer it valid
			is_valid = true;
			for(int i = 1 ; i < numBytes; i++)
			{
				if(buffer[i] < '0' || buffer[i] > '9')
				{
					machine->WriteRegister(2, res);
					delete buffer;
					is_valid = false;
					break;
				}
			}
			if(!is_valid)
			{
				break;
			}
			//
			if(buffer[0] == '-')
			{
				soAm = true;
				index = 1;
			}
			else if(buffer[0] < '0' || buffer[0] > '9')
			{
				machine->WriteRegister(2, res);
				delete buffer;
				break;
			}
			for(int i = index ; i < numBytes; i++)
			{
				res = res * 10 + (int)(buffer[i] - 48);
			}
			if(soAm)
			{
				res *= -1;
			}
			machine->WriteRegister(2, res);
			delete buffer;
			break;
		case SC_WriteInt:
			char* buff;
			int num;
			int leng;
			num = machine->ReadRegister(4); //  doc so nguyen tu thanh ghi r4
			if(num == 0)
			{
				gSynchConsole->Write("0", 1);
				break;
			}

			leng = (int)log10(abs(num)) + 1;
			
			// turn number to char* to print to console
			buff = new char[leng + 1];
			if(num < 0)
			{
				buff[0] = '-';
				num = -num;
				leng++;
			}
			for (int k = leng - 1; k >= 0; k--) 
			{
        			buff[k] = (char)((num % 10) + 48);
        			num /= 10;
				if(num == 0)
					break;
   			}
			buff[leng] = '\0';
			gSynchConsole->Write(buff, leng + 1);
			delete buff;
			break;
		case SC_ReadChar:
			char* buffer_3;
			char res_c;
			buffer_3 = new char[2];
			gSynchConsole->Read(buffer_3, 1);
			buffer_3[1] = '\0';
			res_c = buffer_3[0];
			machine->WriteRegister(2, res_c);
			delete buffer_3;
			break;
		case SC_WriteChar:
			char c;
			c = machine->ReadRegister(4);
			char* buffer_4;
			buffer_4 = new char[2];
			buffer_4[0] = c;
			buffer_4[1] = '/0';
			gSynchConsole->Write(buffer_4, 1);
			break;
		case SC_ReadString:
			int virAddr;
			char* buff_5;
			virAddr = machine->ReadRegister(4);
			leng = machine->ReadRegister(5);
			buff_5 = new char[leng];
			gSynchConsole->Read(buff_5, leng);
			System2User(virAddr, leng, buff_5);
			delete buff_5;
			break;
		case SC_WriteString:
			char* buff_6;
			int index_str;
			virAddr = machine->ReadRegister(4);
			//buff_6 = new char[max_buff];
			buff_6 = User2System(virAddr, 255);
			index_str = 0;		
			while(buff_6[index_str] != '\0' && index_str < 255)
			{
				index_str++;
			}
			gSynchConsole->Write(buff_6,index_str + 1);
			//delete buff_6;
		case SC_Create:
			{
			int bufAddr = machine->ReadRegister(4); // read string pointer from user
			char *buf = new char[max_buff];
			buf = User2System(bufAddr, max_buff);
			// create a new file 
			if (fileSystem->Create(buf, 0) == false)
			{
				DEBUG('f',"can not create file");
				machine->WriteRegister(2, -1);
				} else 
				{
					DEBUG('f',"create file successfully");
					machine->WriteRegister(2, 0);
				};
				delete [] buf;
				break;
			}
		case SC_Open:
		{
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
			int type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
			char* filename;
			filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
			//Kiem tra xem OS con mo dc file khong
			
			// update 4/1/2018
			int freeSlot = fileSystem->FindFreeSlot();
			if (freeSlot != -1) //Chi xu li khi con slot trong
			{
				if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
				{
					
					if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
					{
						machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
					}
				}
				else if (type == 2) // xu li stdin voi type quy uoc la 2
				{
					machine->WriteRegister(2, 0); //tra ve OpenFileID
				}
				else // xu li stdout voi type quy uoc la 3
				{
					machine->WriteRegister(2, 1); //tra ve OpenFileID
				}
				delete[] filename;
				break;
			}
			machine->WriteRegister(2, -1); //Khong mo duoc file return -1
			
			delete[] filename;
			break;

		}
		case SC_Close:
		{
			int fid = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
			if (fid >= 0 && fid <= 10) 
			{
				if (fileSystem->openf[fid])
				{
					delete fileSystem->openf[fid]; //Xoa vung nho luu tru file
					fileSystem->openf[fid] = NULL; //Gan vung nho NULL
					machine->WriteRegister(2, 0);
					break;
				}
			}
			machine->WriteRegister(2, -1);
			break;
		}
		case SC_Read:
		{
			int virtualAddr;
			int size_buff;
			int ID, startPos, endPos;
			char* content;
			int size;
			virtualAddr = machine->ReadRegister(4);
			size_buff = machine->ReadRegister(5);
			ID = machine->ReadRegister(6);
			if(ID < 0 || ID > 9)
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID] == NULL)
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID]->type == 3) // stdin
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			startPos = fileSystem->openf[ID]->GetCurrentPos();
			content = new char[max_buff];
			if(fileSystem->openf[ID]->type == 2)
			{
				size = gSynchConsole->Read(content, size_buff);
				System2User(virtualAddr, size, content);
				machine->WriteRegister(2, size);
				delete content;
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID]->Read(content, size_buff) > 0)
			{
				endPos = fileSystem->openf[ID]->GetCurrentPos();
				System2User(virtualAddr, endPos - startPos, content);
				machine->WriteRegister(2, endPos - startPos);
				printf("Content in file: %s \n", content);
			}
			else 
			{
				machine->WriteRegister(2, -2);
			}
			delete content;	
			IncreasePC();
			return;
		}
		case SC_Write:
		{
			int virtualAddr;
			int size_buff;
			int ID, startPos, endPos;
			char* content;
			int l;
			int size_of_content;
			size_of_content = 0;
			virtualAddr = machine->ReadRegister(4);
			size_buff = machine->ReadRegister(5);
			ID= machine->ReadRegister(6);
			if(ID < 0 || ID > 9)
			{
				printf("Write file failed ! 1\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID] == NULL)
			{
				printf("%d \n", ID);
				printf("Write file failed ! 2\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID]->type == 2 || fileSystem->openf[ID]->type == 1)
			{
				printf("Write file failed ! 3\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			startPos = fileSystem->openf[ID]->GetCurrentPos();
			content = User2System(virtualAddr, size_buff);
			if(fileSystem->openf[ID]->type == 3) // stdout
			{
				int i;
				i = 0;
				while(content[i] != 0 && content[i] != '\n')
				{
					gSynchConsole->Write(content + i, 1);
					++i;
				}
				content[i] = '\n';
				gSynchConsole->Write(content + i, 1);
				machine->WriteRegister(2, i - 1); // return atual size
				delete content;
				IncreasePC();
				return;
			}
			l = 0;
			while(content[l] != '\0')
			{
				l++;
			}
			if(fileSystem->openf[ID]->type == 0)
			{
				if(fileSystem->openf[ID]->Write(content, l) > 0)
				{
					printf("Write file succeed ! \n");
					endPos = fileSystem->openf[ID]->GetCurrentPos();
					machine->WriteRegister(2, endPos - startPos);
					delete content;
					IncreasePC();
					return;
				}
			}
			
		}
		case SC_Seek:
		{
			int pos = machine->ReadRegister(4);
			int ID = machine->ReadRegister(5);
			if(ID < 0 || ID > 9)
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(fileSystem->openf[ID] == NULL)
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(ID == 0 || ID == 1) // stdin/out
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if(pos == -1)
			{
				pos = fileSystem->openf[ID]->Length();
			}
			if(pos > fileSystem->openf[ID]->Length() || pos < 0)
			{
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			else
			{
				fileSystem->openf[ID]->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}
		default:		
			break;
		}
		IncreasePC();
	}
}

