
#include "imgLoad.h"
#include "image.h"
#include "log.h"
#include "gls.h"

#include <vector>

#ifdef ENGINE_JPEG
#include <jpeglib.h>
#endif

namespace IO {

#if defined(ENGINE_JPEG)

	static void init_source(j_decompress_ptr /*cinfo*/)
	{}

	static boolean fill_input_buffer(j_decompress_ptr cinfo)
	{
		struct jpeg_source_mgr * src = cinfo->src;
		static JOCTET FakeEOI[] = { 0xFF, JPEG_EOI };

		// Generate warning
		Log::instance()->printMessage("jpeglib: Premature end of file\n");

		// Insert a fake EOI marker
		src->next_input_byte = FakeEOI;
		src->bytes_in_buffer = 2;

		return TRUE;
	}

	static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
	{
		struct jpeg_source_mgr * src = cinfo->src;

		if (num_bytes >= (long)src->bytes_in_buffer) {
			fill_input_buffer(cinfo);
			return;
		}

		src->bytes_in_buffer -= num_bytes;
		src->next_input_byte += num_bytes;
	}

	static void term_source(j_decompress_ptr /*cinfo*/)
	{
		// no work necessary here
	}

	//jpeg
	struct my_error_mgr {
		struct jpeg_error_mgr pub;	/* "public" fields */

		jmp_buf setjmp_buffer;	/* for return to caller */
	};

	typedef struct my_error_mgr * my_error_ptr;

	METHODDEF(void) my_error_exit(j_common_ptr cinfo)
	{
		/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
		my_error_ptr myerr = (my_error_ptr)cinfo->err;

		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		(*cinfo->err->output_message) (cinfo);

		/* Return control to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
#endif // defined(ENGINE_JPEG)


	base::Image* ImageFile::loadJPG(StdInputStream * stream)
	{
#ifdef ENGINE_JPEG

		std::vector<uint8> byte_array;
		byte_array.resize(stream->size());
		stream->serializeData(byte_array.data(), stream->size());

		jpeg_decompress_struct cinfo;
		jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = my_error_exit;
		jpeg_create_decompress(&cinfo);

		cinfo.src = (struct jpeg_source_mgr *) (*cinfo.mem->alloc_small)
			((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(struct jpeg_source_mgr));
		cinfo.src->init_source = init_source;
		cinfo.src->fill_input_buffer = fill_input_buffer;
		cinfo.src->skip_input_data = skip_input_data;
		cinfo.src->resync_to_restart = jpeg_resync_to_restart;	// use default method
		cinfo.src->term_source = term_source;
		cinfo.src->bytes_in_buffer = byte_array.size();
		cinfo.src->next_input_byte = byte_array.data();

		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		/*
		cinfo.do_fancy_upsampling = FALSE;	// fast decompression
		cinfo.dct_method = JDCT_FLOAT;			// Choose floating point DCT method.
		*/

		uint8 * tmp_buffer = new uint8[cinfo.output_width * cinfo.output_height * cinfo.output_components];
		uint8 * scanline = tmp_buffer;

		while (cinfo.output_scanline < cinfo.output_height) 
		{
			int num_scanlines = jpeg_read_scanlines(&cinfo, &scanline, 1);
			scanline += num_scanlines * cinfo.output_width * cinfo.output_components;
		}

		jpeg_finish_decompress(&cinfo);

		base::Image* img = new base::Image();

		img->allocate(cinfo.output_width, cinfo.output_height, cinfo.output_components);
		img->levelDataPtr_ = new uint8*[1];
		img->levelDataPtr_[0] = (uint8*)img->pixels();

		img->settarget(GL_TEXTURE_2D);
		img->settype(GL_UNSIGNED_BYTE);

		
		uint8 * dst = (uint8*)img->pixels();
		const int size = img->height() * img->width();
		const uint8 * src = tmp_buffer;

		J_COLOR_SPACE cs = cinfo.out_color_space;
		if (cs == JCS_RGB)
		{
			if (cinfo.num_components == 3)
			{
				img->setformat(GL_RGB);
				img->setinternalformat(GL_RGB8); 

				for (int i = 0; i < size; i++)
				{
					*dst++ = src[0];
					*dst++ = src[1];
					*dst++ = src[2];

					src += 3;
				}
			}
			else
			{

			}
		}
		else if (cs == JCS_GRAYSCALE)
		{
			if (cinfo.num_components == 1)
			{
				img->setformat(GL_RED);
				img->setinternalformat(GL_R8);
				for (int i = 0; i < size; i++)
				{
					*dst++ = src[0];
					src += 1;
				}
			}
		}
		else //CMYK
		{
		}

		jpeg_destroy_decompress(&cinfo);

		delete[] tmp_buffer;

		return img;
#else
		Log::instance()->printMessage("have no the jpeg lib ....\n");
		return false;
#endif // ENGINE_JPEG

	}
}
