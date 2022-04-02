#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

uint32_t bytes_to_int(unsigned char *bytes)
{
	uint32_t val = 0;
	for (int i = 0; i < 4; i++)
	{
		val <<= 8; // shift 8 positions to the right
		val |= *bytes++;
	}
	return val;
}

int main()
{
	FILE *ptr;
	ptr = fopen("samples", "rb");
	unsigned char jpeg_header[3] = {0xFF, 0xD8, 0xFF};
	unsigned char jpeg_trailer[2] = {0xFF, 0xD9};
	unsigned char bmp_header[2] = {0x42, 0x4D};

	FILE *writer = fopen("output.JPG", "wb");
	FILE *writer1 = fopen("output.BMP", "wb");

	unsigned char size_bmp[4];

	unsigned char cur;

	long long int start = 0;

	if (ptr == NULL)
	{
		printf("file %s cant be opened\n", "samples");
		exit(0);
	}
	else
	{
		printf("file %s opened\n", "samples");
	}

	int j;

	while (!feof(ptr))
	{
		j = 0;
		start++;
		fread(&cur, 1, 1, ptr);
		if (cur == jpeg_header[j] && j == 0)
		{
			j++;
			start++;
			fread(&cur, 1, 1, ptr);
			if (cur == jpeg_header[j] && j == 1)
			{
				j++;
				start++;
				fread(&cur, 1, 1, ptr);
				if (cur == jpeg_header[j] && j == 2)
				{
					j++;
					printf("%x ---> end of header \n", cur);
					if (j == 3)
					{
						printf("%s\n", "jpeg_header found");
						printf("%lld\n", start);
						break;
					}
				}
				j = 0;
			}
			j = 0;
		}

		else
		{
			j = 0;
		}
	}

	int k;

	fwrite(&jpeg_header, sizeof(jpeg_header), 1, writer);

	printf("%s\n", "JPEG HEX DATA");

	while (!feof(ptr))
	{
		k = 0;
		start++;
		fread(&cur, 1, 1, ptr);
		fwrite(&cur, sizeof(cur), 1, writer);
		if (cur == jpeg_trailer[k])
		{
			k++;
			start++;
			fread(&cur, 1, 1, ptr);
			fwrite(&cur, sizeof(cur), 1, writer);
			if (cur == jpeg_trailer[k])
			{
				printf("%x ---> end of trailer \n", cur);
				k++;
				if (k == 2)
				{
					printf("%s\n", "jpeg_trailer found");
					printf("%lld\n", start);
					break;
				}
			}
			k = 0;
		}
		else
		{
			k = 0;
		}
	}

	int l;
	fseek(ptr, 0, SEEK_SET);
	long bmp_start;
	int bmp_size;
	while (!feof(ptr))
	{
		l = 0;

		fread(&cur, 1, 1, ptr);
		if (cur == bmp_header[l])
		{
			l++;
			fread(&cur, 1, 1, ptr);
			if (cur == bmp_header[l] && (ftell(ptr) - 2) % 16 == 0)
			{
				bmp_start = ftell(ptr) - 2;
				l++;
				if (l == 2)
				{
					printf("%s at %ld\n", "bmp_header found", bmp_start);
					// break;
				}

				fread(&cur, 1, 1, ptr);
				size_bmp[3] = cur;
				fread(&cur, 1, 1, ptr);
				size_bmp[2] = cur;
				fread(&cur, 1, 1, ptr);
				size_bmp[1] = cur;
				fread(&cur, 1, 1, ptr);
				size_bmp[0] = cur;

				printf("size in little endian : ");

				for (int i = 0; i < 4; i++)
				{
					printf("%x", size_bmp[3 - i]);
				}

				bmp_size = bytes_to_int(size_bmp);
				// printf("\ndecimal value of size: %d\nptr = %ld", bmp_size, bmp_start);

				break;
			}
			l = 0;
		}
		else
		{
			l = 0;
		}
	}

	// put the bmp file data into a .bmp file and save it
	fseek(ptr, bmp_start, SEEK_SET);
	char c;
	while (bmp_size != 0)
	{
		fread(&c, sizeof(c), 1, ptr);
		fwrite(&c, sizeof(c), 1, writer1);
		bmp_size--;
	}
	return 0;
}
