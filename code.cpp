#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

#include "decode_all.cpp"
#include "encode_all.cpp"


int main(int argc, char *argv[])
{
	class EncodeInfo encode;
	class DecodeInfo decode;

	if (argc < 4)
	{
		printf("Please provide propert command line arguments\n");
		return e_failure;
	}

	int result = encode.read_and_validate_encode_args(argv, &encode);

	if (result == e_encode)
	{
		result = encode.open_encode_files(&encode);

		if (result == e_success)
			result = encode.do_encoding(&encode);

		if (result == e_success)
			printf("## Encoding done successfully ##\n");
	}
	else if (result == e_decode)
	{
		result = decode.do_decoding(argv, &decode);
		if(result == e_success)
			printf("INFO: ##Decoding succesfull##\n");
		else
			printf("INFO: failed");
	}
}


