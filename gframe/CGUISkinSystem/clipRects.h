#ifndef _CLIP_RECTS_H_
#define _CLIP_RECTS_H_

#include <rect.h>

namespace irr
{

//! Workaround for a bug in IVideoDriver::draw2DImage( ITexture* tex, rect<s32> dstRect, rect<s32> srcRect, rect<s32>* clip, SColor* colors, bool alpha )
//! It modifies dstRect and srcRect so the resulting dstRect is entirely inside the clipping rect.
//! srcRect is scaled so the same part of the image is displayed.
//! Returns false if dstRect is entirely outside clip, or true if at least some of it is inside.
inline bool clipRects( core::rect<s32>& dstRect, core::rect<s32>& srcRect, const core::rect<s32>& clip )
{
    // Clip left side
    if ( dstRect.UpperLeftCorner.X < clip.UpperLeftCorner.X )
    {
        s32 w = clip.UpperLeftCorner.X - dstRect.UpperLeftCorner.X;
        
        f32 percentRemoved = (f32)w / (f32)dstRect.getWidth();
        
        dstRect.UpperLeftCorner.X = clip.UpperLeftCorner.X;
        
        srcRect.UpperLeftCorner.X += (s32)(percentRemoved * srcRect.getWidth());
    }
    
    // Clip top side
    if ( dstRect.UpperLeftCorner.Y < clip.UpperLeftCorner.Y )
    {
        s32 w = clip.UpperLeftCorner.Y - dstRect.UpperLeftCorner.Y;
        
        f32 percentRemoved = (f32)w / (f32)dstRect.getHeight();
        
        dstRect.UpperLeftCorner.Y = clip.UpperLeftCorner.Y;
        
        srcRect.UpperLeftCorner.Y += (s32)(percentRemoved * srcRect.getHeight());
    }
    
    // Clip right side
    if ( dstRect.LowerRightCorner.X > clip.LowerRightCorner.X )
    {
        s32 w = dstRect.LowerRightCorner.X - clip.LowerRightCorner.X;
        
        f32 percentRemoved = (f32)w / (f32)dstRect.getWidth();
        
        dstRect.LowerRightCorner.X = clip.LowerRightCorner.X;
        
        srcRect.LowerRightCorner.X -= (s32)(percentRemoved * srcRect.getWidth());
    }
    
    // Clip bottom side
    if ( dstRect.LowerRightCorner.Y > clip.LowerRightCorner.Y )
    {
        s32 w = dstRect.LowerRightCorner.Y - clip.LowerRightCorner.Y;
        
        f32 percentRemoved = (f32)w / (f32)dstRect.getHeight();
        
        dstRect.LowerRightCorner.Y = clip.LowerRightCorner.Y;
        
        srcRect.LowerRightCorner.Y -= (s32)(percentRemoved * srcRect.getHeight());
    }
    
    return ( dstRect.getWidth() > 0 && dstRect.getHeight() > 0 );
}

} // namespace irr

#endif
