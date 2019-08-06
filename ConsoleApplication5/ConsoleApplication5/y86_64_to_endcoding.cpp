﻿#pragma warning(disable : 4996)

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<cinttypes>

const int Label_n = 20;
// Y86-64 -> 기계어 변환기

/*
스페이스바, 개행문자를 기준으로 읽기때문에
주석, 인스트럭션 ,레지스터들 사이에는 스페이스바가 하나씩을 들어가야합니다.

*/



/*

halt 00
nop  10

rrmovq 20
cmovle 21
cmovl 2 2
cmove 2 3
cmovne 2 4
cmovge 2 5
cmovg 2 6

irmovq 30
rmmovq 40
mrmovq 50


addq 6 0
subq 6 1
andq 6 2
xorq 6 3

jmp 70
jle 71
jl 7 2
je 7 3
jne 7 4
jge 7 5
jg 7 6

call 80
ret 90
pushq A0
popq B0


"%rax",
	"%rcx",
	"%rdx",
	"%rbx",
	"%rsp",
	"%rbp",
	"%rsi",
	"%rdi",
	"%r8",
	"%r9",
	"%r10",
	"%r11",
	"%r12",
	"%r13",
	"%r14",
	"No register"

*/

enum Instruction
{
	halt = 0,
	nop,
	//20

	rrmovq,
	cmovle,
	cmovl,
	cmove,
	cmovne,
	cmovge,
	cmovg,


	irmovq,
	rmmovq,
	mrmovq,


	//60
	addq,
	subq,
	andq,
	xorq,

	//70
	jmp,
	jle,
	jl,
	je,
	jne,
	jge,
	jg,


	//80
	call,
	ret,
	pushq,
	popq

};

const char* instruction[] =
{
	"halt",
	"nop",

	"rrmovq",
	"cmovle",
	"cmovl",
	"cmove",
	"cmovne",
	"cmovge",
	"cmovg",

	"irmovq",
	"rmmovq",
	"mrmovq",


	"addq",
	"subq",
	"andq",
	"xorq",

	"jmp" ,
	"jle" ,
	"jl" ,
	"je" ,
	"jne" ,
	"jge" ,
	"jg" ,

	"call",
	"ret",
	"pushq",
	"popq"
};

const int Instruction_Encoding[]
{
	0x00,
	0x10,
	0x20,
	0x21,
	0x22,
	0x23,
	0x24,
	0x25,
	0x26,

	0x30,
	0x40,
	0x50,

	0x60,
	0x61,
	0x62,
	0x63,

	0x70,
	0x71,
	0x72,
	0x73,
	0x74,
	0x75,
	0x76,

	0x80,
	0x90,
	0xA0,
	0xB0,
};

//register specifier
const char* reg[] =
{
	"%rax",
	"%rcx",
	"%rdx",
	"%rbx",
	"%rsp",
	"%rbp",
	"%rsi",
	"%rdi",
	"%r8",
	"%r9",
	"%r10",
	"%r11",
	"%r12",
	"%r13",
	"%r14",
	"No register"
};

uint64_t reg_value[16];



char Label[Label_n][Label_n];
int Label_index;

uint64_t Label_address[Label_n];

void insrtruction_to_encoding();
void input_error();
uint64_t little_endian(uint64_t integer);
uint64_t get_integer();

void del_comment();

const char* assembler_directives[] =
{
	".pos" ,
	".align",
	".quad"
};

void Print_Encoding_Instruction(int number);
int find_reg(char reg[]);
void reg_in();
void read_label();
uint64_t address = 0x100;
FILE* file = stdin;

int main()
{

	FILE* TXT = fopen("reg.txt", "r") ;
	file = TXT;
		
	read_label();
	rewind(file);
	
	while (!feof(file))
	{
		insrtruction_to_encoding();
	}
	return 0;

}

void insrtruction_to_encoding()
{
	char __instruction[10] = { 0, };
	const int buffer = 10;
	int eof = fscanf_s(file,"%s", __instruction, sizeof(__instruction));

	int ins_len = strlen(__instruction);
	// # 챙기기.
	
	if(__instruction[0] == '#')
	{
		del_comment();
		

		printf("\n");
		return;
	}
	else if (__instruction[0] == '.')
	{
		int i = 0;
		for (; i < 3; i++)
		{
			if (strcmp(__instruction, assembler_directives[i]) == 0)
			{
				break;
			}
		}
		uint64_t D;
		fscanf_s(file, "%" PRIX64, &D);
		switch (i)
		{
		case 0: // .pos
		{
			address = D;
			printf("0x%" PRIX64 " : \n", address);
			break;
		}
		case 1: //.align
		{
			address += D - (address % D);
			printf("0x%" PRIX64" : \n", address);
			break;
		}
		case 2: //quad
		{
			//??
			break;
		}
		default://??
			break;
		}
		return;
	}

	int index = 28;
	for (int i = 0; i < 27; i++)
	{
		size_t len = strlen(instruction[i]);
		if (!strncmp(__instruction, instruction[i], len))//같을때 0반환
		{
			index = i;
			break;
		}
	}
	printf("0x%" PRIX64 " : ", address);
	switch (index)
	{

	case halt:
	case nop:
	case ret: //90
		Print_Encoding_Instruction(index);
		printf("\n");
		address += 1;
		break;
		//20
	case rrmovq:
	case cmovle:
	case cmovl:
	case cmove:
	case cmovne:
	case cmovge:
	case cmovg:
		//60
	case addq:
	case subq:
	case andq:
	case xorq:
		Print_Encoding_Instruction(index);
		reg_in();
		reg_in();
		printf("\n");
		address += 2;
		break;
		//30
	case irmovq:  //수정
	{
		Print_Encoding_Instruction(index);
		printf("F");

		uint64_t D = get_integer(); //정수가 안들어오는경우
		D = little_endian(D);


		reg_in();
		printf("%08" PRIX64 "\n", D);
		address += 10;
		break;
	}	//40
	case rmmovq: //수정
	{
		Print_Encoding_Instruction(index);
		reg_in(); //rA


		char line[20];

		fscanf_s(file, "%s", line, sizeof(line)); //(%s입력)
		uint64_t D = atoi(line);
		
		int i = 0;
		for (; line[i] != '('; i++)
		{
			;
		}
		char rB[5] = { 0, };
		for (int j = 0; line[i] != ')' ; j++, i++)
		{
			rB[j] = line[i]; //  line에 (를 읽기때문에 index 는 1부터. 이게아니면 인풋값에러임
		}
		int rB_index = find_reg(rB);

		printf("%X", rB_index);

		printf("%08" PRIx64 "\n", D);

		address += 10;
		break;
	}
	case mrmovq:
	{
		Print_Encoding_Instruction(index);

		char line[20];

		fscanf_s(file, "%s", line, sizeof(line)); //(%s입력)
		uint64_t D = atoi(line);

		int i = 0;
		for (; line[i] != '('; i++)
		{
			;
		}
		char rB[5] = { 0, };
		i++;
		for (int j = 0; line[i] != ')'; j++, i++)
		{
			rB[j] = line[i]; //  line에 (를 읽기때문에 index 는 1부터. 이게아니면 인풋값에러임
		}


		int rB_index = find_reg(rB);

		reg_in();
		printf("%X", rB_index);
		printf("%08" PRIx64 "\n", D);

		address += 10;
		break;
	}
	//70
	case jmp:
	case jle:
	case jl:
	case je:
	case jne:
	case jge:
	case jg:
		//80
	case call:
	{	//주소값을 저장해서 출력해야됨 씹헬
		Print_Encoding_Instruction(index);
		char line[20];
		fscanf_s(file, "%s", line, sizeof(line));
		int line_len = strlen(line);
		
			/*__instruction[ins_len - 1] = '\0';
			strcpy(Label[Label_index], __instruction);
			Label_address[Label_index] = address;
			Label_index++;*/
			line[line_len] = '\0';
			int i = 0;
			for (; i < Label_index; i++)
			{
				if (strcmp(line, Label[i]) == 0)
				{
					break;
				}
			}

			if (i == Label_index)
			{
				input_error();
				return;
			}
			uint64_t _address = Label_address[i];
			_address = little_endian(_address);
			printf("%08" PRIx64, _address);
			printf("\n");
		
		address += 9;
		break;
	}
	case pushq:
	case popq:
		Print_Encoding_Instruction(index);
		reg_in();
		printf("F\n");
		address += 2;
		break;
	default: //라벨 or 인풋 에러
	{
		if (__instruction[ins_len - 1] == ':')
		{
			/*__instruction[ins_len - 1] = '\0';
			strcpy(Label[Label_index], __instruction);
			Label_address[Label_index] = address;
			Label_index++;*/
			printf("\n");
		}
		else
		{
			if (eof == -1)
			{
				return;
			}
			char line[6];
			fscanf_s(file, "%s", line, sizeof(line));

			//input_error();
		}

		
		//printf("\n");


		break;
	}
	}
	del_comment();
}
void input_error()
{
	printf("input error");
	exit(-1);
	return;
}
//irmovq 0x12345678 %rdi



uint64_t little_endian(uint64_t integer)
{
	uint64_t num = integer;
	uint64_t intreturn = 0;
	uint64_t a = 0xFF;

	for (int i = 0; i < 4; i++)
	{
		intreturn = intreturn << 8;
		intreturn += num & a;
		num = num >> 8;
	}
	return intreturn;
}


void Print_Encoding_Instruction(int number)
{
	if (number == halt)
	{
		printf("00");
		return;
	}

	printf("%X", Instruction_Encoding[number]);
	return;
}

int find_reg(char _reg[])
{
	int index = 0;
	for (; index <= 16; index++)
	{
		if (strcmp(_reg, reg[index]) == 0)
		{
			return index;
		}
	} // 상수값 $8g





	input_error();
	return -1;
}

void reg_in() //한개로 생각하고 둘다 처리하자! 쉼표떼기 + 공백처리 오류 젤나기쉬운곳
{
	char line[10] = { 0, }; //라인을 읽어서 순수 reg만 추출해 _reg에 넣는다.
	char _reg[10] = { 0, };

	fscanf_s(file, "%s", line, sizeof(line));

	int line_i = 0;
	// 1 쉼표만 읽은경우 , 2 쉼표 포함해서 레지를 읽을 경우  
	//3. 레지뒤의 주석을 읽은 경우. 4. 맨뒤에 쉼표가 들어간경우.
	if (line[0] == ',' && line[1] == '\0')
	{
		fscanf_s(file, "%s", line, sizeof(line));
	}

	if (line[0] == ',' && line[1] == '\0')
	{
		input_error();
		return;
	}
	else if (line[0] == ',')
	{
		line_i++;
	}

	int reg_index = 0;//입력이 제대로 되지않을시 에러
	for (; (!isspace(line[line_i])) && (line[line_i] != '#') && (line[line_i] != ',') && (line[line_i] != '\0'); line_i++, reg_index++)
	{
		_reg[reg_index] = line[line_i];
	}
	int reg_encoding = find_reg(_reg);
	printf("%X", reg_encoding);
}


uint64_t get_integer()// 실제 메모리에 들어있는값도 읽을수있어야함.!!
{
	
	uint64_t D = 0;
	char a;
	while  (((a = fgetc(file)) != '$') && (a != '('))
	{
		;
	}
	if (a == '$') // 정수인경우
	{
		fscanf_s(file, "%" PRIX64, &D);
		return D;
	}
	//메모리값인경우.
	char _reg[10] = {0,};

	int i = 0;
	while ((a = fgetc(file)) != ')')
	{
		_reg[i] = a;
		i++;
	}

	return reg_value[find_reg(_reg)];
}

void read_label()
{
	uint64_t address = 0x100;

	while (!feof(file))
	{
		char __instruction[10] = { 0, };
		const int buffer = 10;
		int eof = fscanf_s(file ,"%s", __instruction, sizeof(__instruction));
		int ins_len = strlen(__instruction);

		 if (__instruction[0] == '.')
		{
			int i = 0;
			for (; i < 3; i++)
			{
				if (strcmp(__instruction, assembler_directives[i]) == 0)
				{
					break;
				}
			}
			uint64_t D = 1;
			fscanf_s(file, "%" PRIX64, &D);
			switch (i)
			{
			case 0: // .pos
			{
				address = D;
				break;
			}
			case 1: //.align
			{
				
				address += D - (address % D);
				break;
			}
			case 2: //quad
			{
				//??
				break;
			}
			default://??
				break;
			}
			continue;
		}

		int index = 28;
		for (int i = 0; i < 27; i++)
		{
			size_t len = strlen(instruction[i]);
			if (!strncmp(__instruction, instruction[i], len))//같을때 0반환
			{
				index = i;
				break;
			}
		}
		switch (index)
		{
		case halt:
		case nop:
		case ret: //90
			address += 1;
			break;
			//20
		case rrmovq:
		case cmovle:
		case cmovl:
		case cmove:
		case cmovne:
		case cmovge:
		case cmovg:
			//60
		case addq:
		case subq:
		case andq:
		case xorq:
		{
			char line[100];
			fgets(line, 100, file);

			address += 2;
			break;
			//30
		}
		case irmovq:
		{
			char line[100];
			fgets(line, 100, file);

			address += 10;
			break;
		}	//40
		case rmmovq:
		{
			char line[100];
			fgets(line, 100, file);

			address += 10;
			break;
		}
		case mrmovq:
		{
			char line[100];
			fgets(line, 100, file);
			address += 10;
			break;
		}
		//70
		case jmp:
		case jle:
		case jl:
		case je:
		case jne:
		case jge:
		case jg:
			//80
		case call:
		{	char line[100];
			fgets(line, 100, file);
			address += 9;
			break;
		}
		case pushq:
		case popq:
		{	char line[100];
			fgets(line, 100, file);
			address += 2;
			break;
		}
		default: //라벨 먼저 받든지 말든지
		{
			if (__instruction[ins_len - 1] == ':')
			{
				__instruction[ins_len - 1] = '\0';
				strcpy( Label[Label_index] , __instruction);
				Label_address[Label_index] = address;
				Label_index++;
			}
			else
			{
				if (eof == -1)
				{
					return;
				}
				//input_error();
			}
		}
		}
		del_comment();
	}
}

void del_comment()
{
	char a;
	while ( ((a= fgetc(file)) != '\n') && ( a != EOF))
	{
		;
	}
}
