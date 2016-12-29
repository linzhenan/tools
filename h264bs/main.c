#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define min(a, b) (a < b ? a : b)

enum NALUType
{
	Unspecified0 = 0,
	Coded_slice_of_a_non_IDR_picture,
	Coded_slice_data_partition_A,
	Coded_slice_data_partition_B,
	Coded_slice_data_partition_C,
	Coded_slice_of_an_IDR_picture,
	Supplemental_enhancement_information,
	Sequence_parameter_set,
	Picture_parameter_set,
	Access_unit_delimiter,
	End_of_sequence,
	End_of_stream,
	Filler_data,
	Sequence_parameter_set_extension,
	Prefix_NAL_unit,
	Subset_sequence_parameter_set,
	Depth_parameter_set,
	Reserved17,
	Reserved18,
	Coded_slice_of_an_auxiliary_coded,
	Coded_slice_extension,
	Coded_slice_extension_for_a_depth_view_component_or_a_3D_AVC_texture_view_component,
	Reserved22,
	Reserved23,
	Unspecified24,
	Unspecified25,
	Unspecified26,
	Unspecified27,
	Unspecified28,
	Unspecified29,
	Unspecified30,
	Unspecified31,
};

#define enum2str(e) ""#e""
char *startcode2str(uint8_t start_code)
{
	char *str = "";
	switch (start_code)
	{
	case Unspecified0: str = enum2str(Unspecified0); break;
	case Coded_slice_of_a_non_IDR_picture: str = enum2str(Coded_slice_of_a_non_IDR_picture); break;
	case Coded_slice_data_partition_A: str = enum2str(Coded_slice_data_partition_A); break;
	case Coded_slice_data_partition_B: str = enum2str(Coded_slice_data_partition_B); break;
	case Coded_slice_data_partition_C: str = enum2str(Coded_slice_data_partition_C); break;
	case Coded_slice_of_an_IDR_picture: str = enum2str(Coded_slice_of_an_IDR_picture); break;
	case Supplemental_enhancement_information: str = enum2str(Supplemental_enhancement_information); break;
	case Sequence_parameter_set: str = enum2str(Sequence_parameter_set); break;
	case Picture_parameter_set: str = enum2str(Picture_parameter_set); break;
	case Access_unit_delimiter: str = enum2str(Access_unit_delimiter); break;
	case End_of_sequence: str = enum2str(End_of_sequence); break;
	case End_of_stream: str = enum2str(End_of_stream); break;
	case Filler_data: str = enum2str(Filler_data); break;
	case Sequence_parameter_set_extension: str = enum2str(Sequence_parameter_set_extension); break;
	case Prefix_NAL_unit: str = enum2str(Prefix_NAL_unit); break;
	case Subset_sequence_parameter_set: str = enum2str(Subset_sequence_parameter_set); break;
	case Depth_parameter_set: str = enum2str(Depth_parameter_set); break;
	case Reserved17: str = enum2str(Reserved17); break;
	case Reserved18: str = enum2str(Reserved18); break;
	case Coded_slice_of_an_auxiliary_coded: str = enum2str(Coded_slice_of_an_auxiliary_coded); break;
	case Coded_slice_extension: str = enum2str(Coded_slice_extension); break;
	case Coded_slice_extension_for_a_depth_view_component_or_a_3D_AVC_texture_view_component: str = enum2str(Coded_slice_extension_for_a_depth_view_component_or_a_3D_AVC_texture_view_component); break;
	case Reserved22: str = enum2str(Reserved22); break;
	case Reserved23: str = enum2str(Reserved23); break;
	case Unspecified24: str = enum2str(Unspecified24); break;
	case Unspecified25: str = enum2str(Unspecified25); break;
	case Unspecified26: str = enum2str(Unspecified26); break;
	case Unspecified27: str = enum2str(Unspecified27); break;
	case Unspecified28: str = enum2str(Unspecified28); break;
	case Unspecified29: str = enum2str(Unspecified29); break;
	case Unspecified30: str = enum2str(Unspecified30); break;
	case Unspecified31: str = enum2str(Unspecified31); break;
	}
	return str;
}
#undef enum2str

int main(int argc, char **argv)
{
	FILE *pf = NULL;
	uint8_t *bs = NULL;
	char *filename = NULL;

	if (argc == 2)
		filename = argv[1];
	else
		goto fail;
	printf("open: %s\n", filename);

	pf = fopen(filename, "r");
	if (!pf)
	{
		printf("fail to open file.\n");
		goto fail;
	}

	fseek(pf, 0, SEEK_END);
	int size = ftell(pf);

	bs = (uint8_t*)malloc(sizeof(uint8_t) * size);
	if (!bs)
	{
		printf("fail to malloc mem.\n");
		goto fail;
	}

	fseek(pf, 0, SEEK_SET);
	fread(bs, 1, size, pf);

	uint32_t buf = 0xFFFFFFFF;
	uint32_t last_buf = 0xFFFFFFFF;
	uint8_t last_start_code = 0;
	uint8_t last_last_start_code = 0;
	uint8_t last_priority = 0;
	int last_start_pos = 0;
	int last_start_code_pos = 0;
	int pos = 0;
	int i_nalu = 0;
	int i_au = 0;
	int b_found = 0;
	char str[256];

	for (pos = 0; pos < min(3, size); pos++)
	{
		buf <<= 8;
		buf |= bs[pos];
	}
	last_buf = buf;

	for (pos = 3; pos < size; pos++)
	{
		buf <<= 8;
		buf |= bs[pos];
		if ((buf & 0xFFFFFF00) == 0x00000100)
		{
			uint8_t priority = (buf & 0x7F) >> 5;
			uint8_t start_code = buf & 0x1F;
			int start_pos;
			if (!(last_buf & 0xFF000000))
				start_pos = pos - 4;
			else
				start_pos = pos - 3;

			if (b_found)
			{
				if (i_nalu == 0 ||
					(
						(last_last_start_code == Coded_slice_of_a_non_IDR_picture ||
							last_last_start_code == Coded_slice_data_partition_A ||
							last_last_start_code == Coded_slice_data_partition_B ||
							last_last_start_code == Coded_slice_data_partition_C ||
							last_last_start_code == Coded_slice_of_an_IDR_picture)
						&&
						(last_start_code == Access_unit_delimiter ||
							last_start_code == Sequence_parameter_set ||
							last_start_code == Picture_parameter_set ||
							last_start_code == Supplemental_enhancement_information ||
							(14 <= last_start_code && last_start_code <= 18) ||
							last_start_code == Coded_slice_of_a_non_IDR_picture ||
							last_start_code == Coded_slice_data_partition_A ||
							last_start_code == Coded_slice_data_partition_B ||
							last_start_code == Coded_slice_data_partition_C ||
							last_start_code == Coded_slice_of_an_IDR_picture
							)
						)
					)
					fprintf(stdout, "AU %-3d ", i_au++);//7.4.1.2.3
				else
					fprintf(stdout, "       ");
			
				char trail_str[9] = { 0 };
				uint8_t trail_byte = bs[start_pos - 1];
				for (int i = 0; i < 8; i++)
					if (trail_byte & (1 << (7 - i)))
						trail_str[i] = '1';
					else
						trail_str[i] = '0';
				fprintf(stdout, "NALU %-3d %u [0x%08X - 0x%08X] %6d B [0x%08X] %-2u 0x%02X(%s) %s\n",
					i_nalu++, last_priority,
					last_start_pos, start_pos - 1, start_pos - last_start_pos,
					last_start_code_pos, last_start_code,
					trail_byte, trail_str,
					startcode2str(last_start_code));			
			}

			b_found = 1;

			last_last_start_code = last_start_code;
			last_start_code = start_code;
			last_priority = priority;
			last_start_pos = start_pos;
			last_start_code_pos = pos;
		}
		last_buf = buf;
	}

	if (b_found)
	{
		if (i_nalu == 0 ||
			(
				(last_last_start_code == Coded_slice_of_a_non_IDR_picture ||
					last_last_start_code == Coded_slice_data_partition_A ||
					last_last_start_code == Coded_slice_data_partition_B ||
					last_last_start_code == Coded_slice_data_partition_C ||
					last_last_start_code == Coded_slice_of_an_IDR_picture)
				&&
				(last_start_code == Access_unit_delimiter ||
					last_start_code == Sequence_parameter_set ||
					last_start_code == Picture_parameter_set ||
					last_start_code == Supplemental_enhancement_information ||
					(14 <= last_start_code && last_start_code <= 18) ||
					last_start_code == Coded_slice_of_a_non_IDR_picture ||
					last_start_code == Coded_slice_data_partition_A ||
					last_start_code == Coded_slice_data_partition_B ||
					last_start_code == Coded_slice_data_partition_C ||
					last_start_code == Coded_slice_of_an_IDR_picture
					)
				)
			)
			fprintf(stdout, "AU %-3d ", i_au++);//7.4.1.2.3
		else
			fprintf(stdout, "       ");

		char trail_str[9] = { 0 };
		uint8_t trail_byte = bs[pos - 1];
		for (int i = 0; i < 8; i++)
			if (trail_byte & (1 << (7 - i)))
				trail_str[i] = '1';
			else
				trail_str[i] = '0';
		fprintf(stdout, "NALU %-3d %u [0x%08X - 0x%08X] %6d B [0x%08X] %-2u 0x%02X(%s) %s\n",
			i_nalu++, last_priority,
			last_start_pos, pos - 1, pos - last_start_pos,
			last_start_code_pos, last_start_code,
			trail_byte, trail_str,
			startcode2str(last_start_code));			
	}

fail:
	if (bs)
		free(bs);
	if (pf)
		fclose(pf);
	return 0;
}
