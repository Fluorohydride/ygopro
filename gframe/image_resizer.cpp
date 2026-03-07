#include "image_resizer.h"
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

namespace ygo {

ImageResizer imageResizer;

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
bool ImageResizer::imageScaleSTB(irr::video::IImage* src, irr::video::IImage* dest) {
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
	if(!srcPtr)
		return false;
	void* destPtr = dest->lock();
	if(!destPtr) {
		src->unlock();
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
			dest->unlock();
			src->unlock();
			return false;
		}
	} else {
		// Reuse samplers but update buffer pointers for the current images
		stbir_set_buffer_ptrs(&cache.resize, srcPtr, srcStride, destPtr, destStride);
	}

	const int ok = stbir_resize_extended(&cache.resize);
	dest->unlock();
	src->unlock();
	return ok != 0;
}

/**
 * Scale image using nearest neighbor anti-aliasing.
 * Function by Warr1024, from https://github.com/minetest/minetest/issues/2419, modified.
 */
void ImageResizer::imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading) {
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

void ImageResizer::resize(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading) {
	if(imageScaleSTB(src, dest))
		return;
	imageScaleNNAA(src, dest, use_threading);
}

} // namespace ygo
