#include "image_resizer.h"
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace ygo {

ImageResizer imageResizer;

/** Scale image using nearest neighbor anti-aliasing.
 * Function by Warr1024, from https://github.com/minetest/minetest/issues/2419, modified. */
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
	imageScaleNNAA(src, dest, use_threading);
}

} // namespace ygo
