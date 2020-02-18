#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
#include "types.h"

Status DecodeInfo::do_decoding(char *argv[], DecodeInfo *decInfo)
{
	int result;

	printf("INFO: ##Decoding process started##\n");
	printf("INFO: Opening required files\n");

	decInfo -> fptr_src_image = fopen(argv[2], "r");
	decInfo -> src_fname = argv[2];
	decInfo -> decode_fname = argv[3];
	printf("INFO: Opened %s\n", decInfo -> src_fname);

	if (e_success == decode_magic_string(decInfo))
	{
		return e_success;
	}
}

Status DecodeInfo:: decode_magic_string(DecodeInfo *decInfo)
{
	char ch, a = 0;
	int count = 0, result;

	for (int i = 0; i < 54; i++)
		fread(&ch, 1, 1, decInfo -> fptr_src_image);

	printf("INFO: Decoding magic string\n");
	for (int i = 0; i < 2; i++)
	{
		a = 0;
		for (int j = ((sizeof(char) *8) - 1); j >= 0; j--)
		{
			fread(&ch, 1, 1, decInfo -> fptr_src_image);

			if ((ch & 1) == 1)
				a = (1 << j) | a;
			else
				a = (0 << j) | a;
		}

		if (a == MAGIC_STRING[i])
			count++;
	}

	if (count == 2)
	{
		printf("INFO: done\n");
		result = decode_secret_file_extn_size(decInfo -> fptr_src_image, decInfo);
	}
	else
		return e_failure;

	if (result == e_success)
	{
		printf("INFO: Done. Opened all required file\n");
	}
	else
	{
		printf("INFO: ERROR\n");
		return e_failure;
	}

	if (e_success == decode_secret_file_size( decInfo) )
		printf("INFO: Done\n");
	else
	{
		printf("INFO: ERROR\n");
		return e_failure;
	}
	if (e_success == decode_secret_file_data( decInfo))
		printf("INFO: Done\n");
	else
	{
		printf("INFO: ERROR\n");
		return e_failure;
	}

	return e_success;

}

//----------------------------------------------------------------------------------------------------------------------------------------//
Status DecodeInfo::decode_secret_file_extn_size(FILE *fptr_src_image,DecodeInfo *decInfo)
{
	int num = 0;
	char ch, a;

	for (int j = ((sizeof(int) *8) - 1); j >= 0; j--)
	{
		fread(&ch, 1, 1, fptr_src_image);

		if ((ch & 1) == 1)
			num = (1 << j) | num;
		else
			num = (0 << j) | num;
	}

	char ext[num];

	for (int i = 0; i < num; i++)
	{
		a = 0;
		for (int j = ((sizeof(char) *8) - 1); j >= 0; j--)
		{
			fread(&ch, 1, 1, fptr_src_image);

			if ((ch & 1) == 1)
				a = (1 << j) | a;
			else
				a = (0 << j) | a;
		}
		ext[i] = a;
		if(i == (num - 1))
			ext[i + 1] = '\0';
	}

	strcat(decInfo -> decode_fname, ext);
	decInfo -> fptr_decode = fopen( decInfo -> decode_fname , "w");
	printf("INFO: Opened %s\n", decInfo -> decode_fname);

	return e_success;
}

//--------------------------------------------------------------------------------------------------------------------------------------//

Status DecodeInfo::decode_secret_file_size(DecodeInfo *decInfo)
{
	char byte, ch = 0, *ptr;
	int file_size, src_offset = ftell(decInfo -> fptr_src_image);

	printf("INFO: Decoding %s file size\n", decInfo -> decode_fname);
	ptr = (char *)&file_size + 3;
	while (ftell(decInfo -> fptr_src_image) < (src_offset + 32))
	{
		for (int msb = 7; msb >= 0 ; msb--)
		{
			fread(&byte, 1, 1, decInfo -> fptr_src_image);
			ch = ch + ((char)0xFF & ((byte & (char)1) << msb));
		}

		*ptr = ch;
		ptr--;

		ch = 0;
	}

	decInfo -> secret_file_size = file_size;
	return e_success;
}

//----------------------------------------------------------------------------------------------------------------------------------------//

Status DecodeInfo::decode_secret_file_data(DecodeInfo *decInfo) 
{
	char ch, a;

	printf("INFO: Deconding %s file data\n", decInfo -> decode_fname);
	for (int i = 0; i < decInfo -> secret_file_size; i++)
	{
		a = 0;
		for (int j = ((sizeof(char) *8) - 1); j >= 0; j--)
		{
			fread(&ch, 1, 1, decInfo -> fptr_src_image);
			
			if((ch & 1) == 1)
				a = (1 << j) | a;
			else
				a = (0 << j) | a;
		}

		fwrite(&a, 1, 1, decInfo -> fptr_decode);
	}

	return e_success;
}
