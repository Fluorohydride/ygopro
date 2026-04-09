#include "image_utility.h"
#include <cmath>
#include <new>
#include <setjmp.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

extern "C" {
#include <jpeglib.h>
}

namespace ygo {

struct JpegErrorMgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

static void jpeg_error_exit_cb(j_common_ptr cinfo) {
	JpegErrorMgr* err = reinterpret_cast<JpegErrorMgr*>(cinfo->err);
	longjmp(err->setjmp_buffer, 1);
}

struct StbSamplerCache {
	STBIR_RESIZE resize{};
	int in_w = 0;
	int in_h = 0;
	int out_w = 0;
	int out_h = 0;
	stbir_pixel_layout layout = STBIR_BGRA;
	bool samplers_built = false;

	~StbSamplerCache() {
		if(samplers_built) {
			stbir_free_samplers(&resize);
			samplers_built = false;
		}
	}

	void reset_if_needed(int new_in_w, int new_in_h, int new_out_w, int new_out_h, stbir_pixel_layout new_layout) {
		if(new_in_w == in_w && new_in_h == in_h && new_out_w == out_w && new_out_h == out_h && new_layout == layout)
			return;
		if(samplers_built) {
			stbir_free_samplers(&resize);
			samplers_built = false;
		}
		in_w = new_in_w;
		in_h = new_in_h;
		out_w = new_out_w;
		out_h = new_out_h;
		layout = new_layout;
		resize = STBIR_RESIZE{};
	}
};

/**
 * Scale image using stb_image_resize2.
 * Returns true on success, false on failure or unsupported format.
 */
bool ImageUtility::imageScaleSTB(irr::video::IImage* src, irr::video::IImage* dest) {
	if(!src || !dest)
		return false;

	const auto srcDim = src->getDimension();
	const auto destDim = dest->getDimension();
	if(srcDim.Width == 0 || srcDim.Height == 0 || destDim.Width == 0 || destDim.Height == 0)
		return false;
	if(src->getColorFormat() != dest->getColorFormat())
		return false;

	stbir_pixel_layout layout = STBIR_BGRA;
	// Fast-paths (8-bit per channel only):
	// - ECF_A8R8G8B8: Irrlicht stores as BGRA in memory on little-endian.
	// - ECF_R8G8B8: common for JPEGs (3 channels).
	switch(src->getColorFormat()) {
	case irr::video::ECF_A8R8G8B8:
		layout = STBIR_BGRA;
		break;
	case irr::video::ECF_R8G8B8:
		layout = STBIR_RGB;
		break;
	default:
		return false;
	}

	void* srcPtr = src->lock();
	void* destPtr = dest->lock();
	if(!srcPtr || !destPtr) {
		return false;
	}

	const int srcStride = (int)src->getPitch();
	const int destStride = (int)dest->getPitch();

	thread_local StbSamplerCache cache;
	cache.reset_if_needed((int)srcDim.Width, (int)srcDim.Height, (int)destDim.Width, (int)destDim.Height, layout);

	if(!cache.samplers_built) {
		stbir_resize_init(&cache.resize,
						  srcPtr, (int)srcDim.Width, (int)srcDim.Height, srcStride,
						  destPtr, (int)destDim.Width, (int)destDim.Height, destStride,
						  layout, STBIR_TYPE_UINT8);
		stbir_set_edgemodes(&cache.resize, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP);
		// Use box filters to reduce aliasing when downscaling.
		stbir_set_filters(&cache.resize, STBIR_FILTER_BOX, STBIR_FILTER_BOX);
		cache.samplers_built = (stbir_build_samplers(&cache.resize) != 0);
		if(!cache.samplers_built) {
			return false;
		}
	} else {
		// Reuse samplers but update buffer pointers for the current images
		stbir_set_buffer_ptrs(&cache.resize, srcPtr, srcStride, destPtr, destStride);
	}

	return (stbir_resize_extended(&cache.resize) != 0);
}

/**
 * Scale image using nearest neighbor anti-aliasing.
 * Function by Warr1024, from https://github.com/minetest/minetest/issues/2419, modified.
 */
void ImageUtility::imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading) {
	const auto& srcDim = src->getDimension();
	const auto& destDim = dest->getDimension();
	if (destDim.Width == 0 || destDim.Height == 0)
		return;

	// Cache scale ratios.
	const double rx = (double)srcDim.Width / destDim.Width;
	const double ry = (double)srcDim.Height / destDim.Height;

#pragma omp parallel if(use_threading)
{
	// Walk each destination image pixel.
#pragma omp for schedule(dynamic)
	for(irr::s32 dy = 0; dy < (irr::s32)destDim.Height; dy++) {
		for(irr::s32 dx = 0; dx < (irr::s32)destDim.Width; dx++) {
			// Calculate floating-point source rectangle bounds.
			double minsx = dx * rx;
			double maxsx = minsx + rx;
			double minsy = dy * ry;
			double maxsy = minsy + ry;
			irr::u32 sx_begin = (irr::u32)std::floor(minsx);
			irr::u32 sx_end = (irr::u32)std::ceil(maxsx);
			if (sx_end > srcDim.Width)
				sx_end = srcDim.Width;
			irr::u32 sy_begin = (irr::u32)std::floor(minsy);
			irr::u32 sy_end = (irr::u32)std::ceil(maxsy);
			if (sy_end > srcDim.Height)
				sy_end = srcDim.Height;

			// Total area, and integral of r, g, b values over that area,
			// initialized to zero, to be summed up in next loops.
			double area = 0, ra = 0, ga = 0, ba = 0, aa = 0;
			irr::video::SColor pxl, npxl;

			// Loop over the integral pixel positions described by those bounds.
			for(irr::u32 sy = sy_begin; sy < sy_end; sy++) {
				for(irr::u32 sx = sx_begin; sx < sx_end; sx++) {
					// Calculate width, height, then area of dest pixel
					// that's covered by this source pixel.
					double pw = 1;
					if(minsx > sx)
						pw += sx - minsx;
					if(maxsx < (sx + 1))
						pw += maxsx - sx - 1;
					double ph = 1;
					if(minsy > sy)
						ph += sy - minsy;
					if(maxsy < (sy + 1))
						ph += maxsy - sy - 1;
					double pa = pw * ph;

					// Get source pixel and add it to totals, weighted
					// by covered area and alpha.
					pxl = src->getPixel(sx, sy);
					area += pa;
					ra += pa * pxl.getRed();
					ga += pa * pxl.getGreen();
					ba += pa * pxl.getBlue();
					aa += pa * pxl.getAlpha();
				}
			}
			// Set the destination image pixel to the average color.
			if(area > 0) {
				npxl.set((irr::u32)(aa / area + 0.5),
						 (irr::u32)(ra / area + 0.5),
						 (irr::u32)(ga / area + 0.5),
						 (irr::u32)(ba / area + 0.5));
			} else {
				npxl.set(0);
			}
			dest->setPixel(dx, dy, npxl);
		}
	}
} // end of parallel region
}

void ImageUtility::Resize(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading) {
	if(imageScaleSTB(src, dest))
		return;
	imageScaleNNAA(src, dest, use_threading);
}

/**
 * Rotate image counter-clockwise by 90 degrees. (Defense position)
 * @return Image pointer. Must be dropped after use.
 */
irr::video::IImage* ImageUtility::RotateImageCCW90(irr::video::IVideoDriver* driver, irr::video::IImage* src) {
	if(!src || !driver)
		return nullptr;
	irr::core::dimension2d<irr::u32> srcSize = src->getDimension();
	irr::core::dimension2d<irr::u32> destSize(srcSize.Height, srcSize.Width);
	irr::video::IImage* dest = driver->createImage(src->getColorFormat(), destSize);
	void* srcData = src->lock();
	void* destData = dest->lock();
	if(srcData && destData) {
		irr::u32 srcPitch = src->getPitch();
		irr::u32 destPitch = dest->getPitch();
		irr::u32 bytesPerPixel = src->getBytesPerPixel();
		for(irr::u32 y = 0; y < srcSize.Height; ++y) {
			for(irr::u32 x = 0; x < srcSize.Width; ++x) {
				// counter-clockwise 90 degrees: dest(y, W-1-x) = src(x, y)
				irr::u32 srcOffset = y * srcPitch + x * bytesPerPixel;
				irr::u32 destOffset = (srcSize.Width - 1 - x) * destPitch + y * bytesPerPixel;
				memcpy((irr::u8*)destData + destOffset, (irr::u8*)srcData + srcOffset, bytesPerPixel);
			}
		}
	}
	return dest;
}

/**
 * Decode a JPEG file using libjpeg-turbo with optional DCT-domain downscaling (1/2, 1/4, 1/8).
 * When targetWidth / targetHeight are provided, the max scale denominator that keeps the
 * decoded dimensions >= target is chosen, saving memory and CPU time. Also it will output in
 * ECF_A8R8G8B8 format directly, which is the most common format for textures in Irrlicht.
 * The reader is not dropped by this function.
 * @return Image pointer. Must be dropped after use.
 */
irr::video::IImage* ImageUtility::LoadJpegImage(irr::video::IVideoDriver* driver, irr::io::IReadFile* reader, irr::s32 targetWidth, irr::s32 targetHeight) {
	if(!reader) return nullptr;
	const long inputSize = reader->getSize();
	if(inputSize <= 0) return nullptr;
	unsigned char* inputData = new (std::nothrow) unsigned char[inputSize];
	if(!inputData) return nullptr;

	if(reader->read(inputData, inputSize) != inputSize) {
		delete[] inputData;
		return nullptr;
	}

	struct jpeg_decompress_struct cinfo;
	JpegErrorMgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpeg_error_exit_cb;

	unsigned char* volatile outputData = nullptr;

	if(setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		delete[] inputData;
		delete[] outputData;
		return nullptr;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, inputData, (unsigned long)inputSize);

	jpeg_read_header(&cinfo, TRUE);

	// Choose the max scale_denom where decoded size is still >= target
	cinfo.scale_num = 1;
	cinfo.scale_denom = 1;
	if(targetWidth > 0 && targetHeight > 0) {
		const int denoms[] = {8, 4, 2};
		for(int d : denoms) {
			if((int)cinfo.image_width  / d >= targetWidth &&
			   (int)cinfo.image_height / d >= targetHeight) {
				cinfo.scale_denom = d;
				break;
			}
		}
	}

	// CMYK/YCCK JPEGs cannot be converted to RGB directly by libjpeg;
	// decode as CMYK (4 ch) and convert manually using the Adobe inverted convention.
	const bool useCMYK = (cinfo.jpeg_color_space == JCS_CMYK ||
	                      cinfo.jpeg_color_space == JCS_YCCK);
	cinfo.out_color_space = useCMYK ? JCS_CMYK : JCS_EXT_BGRA;

	const bool fastDecoding = cinfo.scale_denom > 2;
	cinfo.do_fancy_upsampling = fastDecoding ? FALSE : TRUE;
	cinfo.do_block_smoothing = fastDecoding ? FALSE : TRUE;
	cinfo.dct_method = fastDecoding ? JDCT_IFAST : JDCT_ISLOW;

	jpeg_start_decompress(&cinfo);

	const size_t width = cinfo.output_width;
	const size_t height = cinfo.output_height;
	const size_t rowspan = width * 4; // ARGB bytes per pixel for output
	const unsigned long long outputSize = (unsigned long long)rowspan * height;
	if (outputSize > SIZE_MAX) { // SIZE_MAX may be smaller than uint64 max on 32-bit systems
		longjmp(jerr.setjmp_buffer, 1);
	}

	// Ownership of outputData will be transferred to the IImage created by createImageFromData()
	outputData = new (std::nothrow) unsigned char[outputSize];
	if (!outputData) {
		longjmp(jerr.setjmp_buffer, 1); 
	}

	if (useCMYK) {
		unsigned char* rowArray[1];
		while (cinfo.output_scanline < cinfo.output_height) {
			rowArray[0] = &outputData[(size_t)cinfo.output_scanline * rowspan];
			jpeg_read_scanlines(&cinfo, rowArray, 1);

			// assume CMYK is in Adobe inverted convention, convert to RGB by multiplying by K
			unsigned char* row = rowArray[0];
			for (size_t i = 0; i < width; i++) {
				const unsigned char c = row[i * 4 + 0];
				const unsigned char m = row[i * 4 + 1];
				const unsigned char y = row[i * 4 + 2];
				const unsigned char k = row[i * 4 + 3];
				row[i * 4 + 3] = 255;				 	// A
				row[i * 4 + 2] = (c * k + 127) / 255;	// R
				row[i * 4 + 1] = (m * k + 127) / 255;	// G
				row[i * 4 + 0] = (y * k + 127) / 255;	// B
			}
		}
	} else {
		unsigned char* rowArray[1];
		while (cinfo.output_scanline < cinfo.output_height) {
			rowArray[0] = &outputData[(size_t)cinfo.output_scanline * rowspan];
			jpeg_read_scanlines(&cinfo, rowArray, 1);
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	delete[] inputData;

	return driver->createImageFromData(
		irr::video::ECF_A8R8G8B8,
		irr::core::dimension2d<irr::u32>(width, height),
		outputData, true, true);
}

} // namespace ygo
