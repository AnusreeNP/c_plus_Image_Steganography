#include "encode.h"
#include "types.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

OperationType EncodeInfo::read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	int k = strcmp (argv[1] , "-e");

	if (k == 0)
	{
		if(strstr(argv[2], ".bmp") == NULL)
		{
			printf("INFO : Src file is not a .bmp file !\n");
			return e_unsupported;
		}
		else if (strstr(argv[3], ".txt") == NULL && strstr(argv[3], ".c") == NULL)
		{
			printf("INFO : Provide Secret file !\n");
			return e_unsupported;
		}
		else if (argv[4] != NULL && strstr(argv[4], ".bmp") == NULL)
		{
			printf("INFO : Dest file not a .bmp file !\n");
			return e_unsupported;
		}

		encInfo -> src_image_fname = argv[2];
		encInfo -> secret_fname = argv[3];
		encInfo -> stego_image_fname = argv[4];
		return e_encode;
	}
	else
	{
		k = strcmp (argv[1], "-d");

		if (k == 0)
		{
			if(strstr(argv[2], ".bmp") == NULL)
			{
				printf("INFO : Src file is not a .bmp file !\n");
				return e_unsupported;
			}
			else if (argv[2] == NULL)
			{
				printf("INFO : Provide Secret file !\n");
				return e_unsupported;
			}

			return e_decode;

		}
		else
			printf("Please provide en/decode type");
	}
}

//--------------------------------------------------------------------------------------------------------------------------------//

Status EncodeInfo::open_encode_files(EncodeInfo *encInfo)
{
	encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");  
	encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "w");  
	encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");  

	printf("INFO: Opening required files\n");
	if (encInfo -> fptr_src_image == NULL)
	{
		printf("INFO: Cannot open src image bmp file !!\n");
		return e_failure;
	}
	else if (encInfo -> fptr_secret == NULL)
	{
		printf("INFO: Cannot open secret file !!\n");
		return e_failure;
	}
	else if (encInfo -> fptr_stego_image == NULL)
	{
		printf("INFO: Cannot open stego file !!\n");
		return e_failure;
	}

	printf("INFO: Opened %s\n", encInfo -> src_image_fname);
	printf("INFO: Opened %s\n", encInfo -> secret_fname);
	printf("INFO: Opened %s\n", encInfo -> stego_image_fname);
	printf("INFO: DONE\n");

	return e_success;
}

//---------------------------------------------------------------------------------------------------------------------------------------//

Status EncodeInfo::do_encoding(EncodeInfo *encInfo)
{
	char *secretf_ext;
	int secretf_ext_size;
	int result; 
	long secretf_size = 0;

	printf("INFO: ## Encoding Procedure Started ##\n");
	printf("INFO: Checking for %s size\n", encInfo -> secret_fname);
	secretf_size = ( get_file_size(encInfo -> fptr_secret) - 1);
	if (secretf_size == 0) 
	{
		printf("INFO: File is empty\n");
		return e_failure;
	}

	rewind(encInfo -> fptr_secret);

	printf("INFO: Done Not Empty\n");

	encInfo -> secret_file_size = secretf_size;

	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

	if (check_capacity(encInfo) == e_success)
		printf("INFO: DONE. FOUND OK\n");
	else
	{
		printf("INFO: Insufficient image capacity for secret file.txt data");
		return e_failure;
	}

	rewind(encInfo -> fptr_secret);

	result = copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image);

	result = encode_magic_string(MAGIC_STRING, encInfo);
	if (result == e_failure)
	{
		printf("INFO: ERROR IN MAGIC STRING\n");
		return e_failure;
	}

	if (e_success == encode_secret_file_extn(encInfo -> secret_fname, encInfo))
	{
		printf("INFO: Done\n");
	}
	else
		printf("INFO: Error\n");

	if (e_success == encode_secret_file_size(encInfo -> secret_file_size, encInfo))
	{
		printf("INFO: Done\n");
	}
	else
		printf("INFO: ERROR\n");

	if (e_success == encode_secret_file_data(encInfo))
	{
		printf("INFO: Done\n");
	}
	else
		printf("INFO: ERROR\n");

	if (e_success == copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image))
	{
		printf("INFO: Done\n");	
	}
	else
		printf("INFO: ERROR\n");

	return e_success;
}

//--------------------------------------------------------------------------------------------------------------------------------------//

long get_file_size(FILE *fptr)
{
	char ch;

	while (1)
	{
		if(feof(fptr))
			break;

		fread(&ch, 1, 1, fptr);
	}

	return ftell(fptr);
}

//---------------------------------------------------------------------------------------------------------------------------------------//

long get_image_size_for_bmp(FILE *fptr_src_image)
{
	uint width, height;
	uint bpp;
	long img_size;

	// Seek to 18th byte
	fseek(fptr_src_image, 18, SEEK_SET);
	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_src_image);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_src_image);

	// Get bits per pixel
	fseek(fptr_src_image, 28, SEEK_SET);
	fread(&bpp, sizeof(short), 1, fptr_src_image);

	img_size = width * height * bpp; // Calculate image size

	rewind(fptr_src_image);

	return img_size;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//

Status check_capacity(EncodeInfo *encInfo)
{
	long int sum = 2 + 4 + 4;

	printf("INFO: Checking for %s to handle %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
	sum += encInfo -> secret_file_size;
	sum = sum * 8;
	sum = sum + 54;

	if ((encInfo -> image_capacity) > sum)
		return e_success;
	else
		return e_failure;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char ch;

	printf("INFO: Copying image header\n");
	for (int i = 0; i < 54; i++)
	{
		fread(&ch, 1, 1, fptr_src_image);
		fwrite(&ch, 1, 1, fptr_stego_image);
	}
	printf("INFO: Done\n");

	return e_success;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	char ch;

	printf("INFO: Encoding Magic string\n");

	for (int i = 0; i < 2; i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			if (1 & (magic_string[i] >> j))
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 1;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}
			else
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 0;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}

		}
	}
	printf("INFO: Done\n");

	return e_success;
}

//--------------------------------------------------------------------------------------------------------------------------------------//

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	char ch;
	char arr[10];
	int chk = 0, j = 0;

	printf("INFO: Encoding %s File Extension\n", encInfo -> secret_fname);
	for (int i = 0; 1; i++)
	{
		if (file_extn[i] == '\0')
		{
			arr[j] = '\0';
			break;
		}

		if (file_extn[i] == '.')
			chk  = 1;

		if(chk)
		{
			arr[j] = file_extn[i];
			j++;
		}
	}

	int size = j;

	for (int i = 31; i >= 0; i--)
	{
		if (1 & (size >> i))
		{
			fread(&ch, 1, 1, encInfo -> fptr_src_image);
			ch = ch & (-1 << 1);
			ch = ch | 1;
			fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
		}
		else
		{
			fread(&ch, 1, 1, encInfo -> fptr_src_image);
			ch = ch & (-1 << 1);
			ch = ch | 0;
			fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
		}
	}

	for (int i = 0; i < size; i++)
	{
		for (int k = 7; k >= 0; k--)
		{
			if (1 & (arr[i] >> k))
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 1;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}
			else
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 0;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}

		}
	}

	return e_success;
}

//--------------------------------------------------------------------------------------------------------------------------------------//

Status encode_secret_file_size(long secret_file_size, EncodeInfo *encInfo)
{
	char ch;

	printf("INFO: Encoding %s file size\n", encInfo -> secret_fname);
	for (int i = 31; i >= 0; i--)
	{
		if (1 & (secret_file_size >> i))
		{
			fread(&ch, 1, 1, encInfo -> fptr_src_image);
			ch = ch & (-1 << 1);
			ch = ch | 1;
			fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
		}
		else
		{
			fread(&ch, 1, 1, encInfo -> fptr_src_image);
			ch = ch & (-1 << 1);
			ch = ch | 0;
			fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
		}
	}

	return e_success;
}

//----------------------------------------------------------------------------------------------------------------------------------------//

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char ch, arr[1000];

	for (int i = 0; 1; i++)
	{
		if (feof(encInfo -> fptr_secret))
		{
			arr[i] = '\0';
			break;
		}

		fread(&ch, 1, 1, encInfo -> fptr_secret);
		arr[i] = ch;
	}

	int size = (strlen(arr) - 2);

	printf("INFO: Encoding %s file data\n", encInfo -> secret_fname);
	for (int i = 0; i < size; i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			if (1 & (arr[i] >> j))
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 1;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}
			else
			{
				fread(&ch, 1, 1, encInfo -> fptr_src_image);
				ch = ch & (-1 << 1);
				ch = ch | 0;
				fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
			}
		}
	}

	return e_success;
}

//-------------------------------------------------------------------------------------------------------------------------------------//

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char ch;

	printf("INFO: Copying leftover data\n");

	while (fread(&ch, 1, 1, fptr_src_image) != 0)
	{
		fwrite(&ch, 1, 1, fptr_stego_image);
	}

	return e_success;
}

