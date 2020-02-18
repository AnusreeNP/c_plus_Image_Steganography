#ifndef DECODE_H
#define DECODE_H

#include "encode.h"
#include "types.h"

class DecodeInfo
{
	public:
	/* Source Image Info */
	FILE *fptr_src_image;
	char *src_fname;

	/* Secret File Info */
	long int secret_file_size;
	int secret_file_extn_size;

	/* Decoded File Info */
	FILE *fptr_decode;
	char *decode_fname;

	/* Function start the decoding process */
	Status do_decoding(char *argv[], DecodeInfo *decInfo);

	/* Function to decode the magic string */
	Status decode_magic_string(DecodeInfo *decInfo);

	/* Function to decode the secret file extension size */
	Status decode_secret_file_extn_size(FILE *fptr_src_image, DecodeInfo *decInfo);

	/* Function to decode the secret file extension */
	Status decode_secret_file_extn(DecodeInfo *decInfo);

	/* Function to decode the secret file size */
	Status decode_secret_file_size(DecodeInfo *decInfo);

	/* Function to decode the secret file data */
	Status decode_secret_file_data(DecodeInfo *decInfo);
};
#endif
