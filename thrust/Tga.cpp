//
// Loads in 32-bit RGBA uncompressed TGAs.
//

#include "always.h"
#include "tga.h"



TGA_Info TGA_load(
			const char* file,
			std::int32_t        max_width,
			std::int32_t        max_height,
			TGA_Pixel   *data)
{
	std::int32_t i;

	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;

	std::int32_t tga_width;
	std::int32_t tga_height;
	std::int32_t tga_pixel_depth;
	std::int32_t tga_image_type;
	std::int32_t tga_id_length;

	std::uint8_t header[18];
	std::uint8_t rubbish;
	std::uint8_t no_alpha;

	FILE *handle;

	TGA_Info ans;

	//
	// Open the file.
	//

	handle = fopen(file, "rb");

	if (!handle )
	{
		TRACE("Could not open TGA file %s", file);
		ans.valid = false;
		return ans;
	}

	//
	// Read the header.
	//

	if (fread(header, sizeof(std::uint8_t), 18, handle) != 18) goto file_error;

	//
	// Extract info from the header.
	//

	tga_id_length   = header[0x0];
	tga_image_type  = header[0x2];
	tga_width       = header[0xc] + header[0xd] * 256;
	tga_height      = header[0xe] + header[0xf] * 256;
	tga_pixel_depth = header[0x10];

	//
	// Is this a valid tga file?
	//

	ans.valid  = false;
	ans.width  = tga_width;
	ans.height = tga_height;
	ans.flag   = 0;

	if (tga_image_type != 2)
	{
		TRACE("Tga must be true colour and uncompressed.\n");
		fclose(handle);
		return ans;
	}

	if (tga_pixel_depth != 32 && tga_pixel_depth != 24)
	{
		TRACE("Tga must be 32-bit or 24-bit (24-bit + 8-bit alpha channel)\n");
		fclose(handle);
		return ans;
	}
	
	if (tga_width  > max_width ||
		tga_height > max_height)
	{
		TRACE("Invalid dimensions:\n\tWanted <= %d x %d\n\tGot       %d x %d\n", max_width, max_height, tga_width, tga_height);
		fclose(handle);
		return ans;
	}

	//
	// The tga file is valid...
	//

	ans.valid = true;

	//
	// Skip past the image identification field.
	//
	
	for (i = 0; i < tga_id_length; i++)
	{
		if (fread(&rubbish, sizeof(std::uint8_t), 1, handle) != 1) goto file_error;
	}

	//
	// Load in the data.
	//

	if (tga_pixel_depth == 32)
	{
		if (fread(data, sizeof(TGA_Pixel), tga_width * tga_height, handle) != tga_width * tga_height) goto file_error;

		no_alpha = false;
	}
	else
	{
		//
		// We have to load a pixel in at a time to add the nullptr alpha channel.
		//

		for (i = 0; i < tga_width * tga_height; i++)
		{
			if (fread(&blue,  sizeof(std::uint8_t), 1, handle) != 1) goto file_error;
			if (fread(&green, sizeof(std::uint8_t), 1, handle) != 1) goto file_error;
			if (fread(&red,   sizeof(std::uint8_t), 1, handle) != 1) goto file_error;
			
			data[i].red   = red;
			data[i].green = green;
			data[i].blue  = blue;
			data[i].alpha = 255;
		}

		no_alpha = true;
	}

	fclose(handle);

	//
	// Loaded in the tga. Sets the flags- is it grayscale?
	//

	if (!no_alpha)
	{
		ans.flag |= TGA_FLAG_ONE_BIT_ALPHA;

		for (i = 0; i < tga_width * tga_height; i++)
		{
			if (data[i].alpha != 255)
			{
				//
				// Found alpha channel data.
				//

				ans.flag |= TGA_FLAG_CONTAINS_ALPHA;

				if (ans.flag != 0)
				{
					ans.flag &= ~TGA_FLAG_ONE_BIT_ALPHA;

					break;
				}
			}
		}

		if (!(ans.flag & TGA_FLAG_CONTAINS_ALPHA))
		{
			ans.flag &= ~TGA_FLAG_ONE_BIT_ALPHA;
		}
	}

	//
	// Is it grayscale?
	//

	ans.flag |= TGA_FLAG_GRAYSCALE;

	for (i = 0; i < tga_width * tga_height; i++)
	{
		if (data[i].red   != data[i].green ||
			data[i].red   != data[i].blue  ||
			data[i].green != data[i].blue)
		{
			ans.flag &= ~TGA_FLAG_GRAYSCALE;

			break;
		}
	}

	return ans;

  file_error:;

	//
	// Error!
	//

	TRACE("File error loading TGA file %s\n", file);
	fclose(handle);
	ans.valid = false;

	return ans;
}








std::uint8_t TGA_header[18] =
{
	0, 0, 2, 0,
	0, 0, 0, 0, 
	0, 0, 0, 0, 
	0, 1,	// Width  LSB:MSB
	0, 1, 	// Height LSB:MSB
	24,		// Pixel depth
	0
};

void TGA_save(
		const char* file,
		std::int32_t        width,
		std::int32_t        height,
		TGA_Pixel   *data,
		std::int32_t contains_alpha)
{
	std::int32_t x;
	std::int32_t y;

	std::int32_t num_pixels;
	std::uint8_t header[18];
	std::int32_t bpp;

	FILE *handle;

	handle = fopen(file, "wb");

	if (!handle )
	{
		TRACE("Cannot open TGA file %s\n", file);

		return;
	}

	//
	// Create the header.
	//

	std::int32_t i;

	for (i = 0; i < 18; i++)
	{
		header[i] = TGA_header[i];
	}

	header[0xc] = width  & 0xff;
	header[0xd] = width  >> 8;
	header[0xe] = height & 0xff;
	header[0xf] = height >> 8;

	header[0x10] = (contains_alpha) ? 32 : 24;

	//
	// Write out the header.
	//

	fwrite(&header, sizeof(std::uint8_t), 18, handle);

	//
	// Write out the pixel data.
	//

	for (y = width - 1; y >=     0; y--)
	for (x =         0; x <  width; x++)
	{
		if (contains_alpha)
		{
			fwrite(&data[x + y * width].alpha, sizeof(std::uint8_t), 1, handle);
		}

		fwrite(&data[x + y * width].blue,  sizeof(std::uint8_t), 1, handle);
		fwrite(&data[x + y * width].green, sizeof(std::uint8_t), 1, handle);
		fwrite(&data[x + y * width].red,   sizeof(std::uint8_t), 1, handle);
	}

	fclose(handle);
}
