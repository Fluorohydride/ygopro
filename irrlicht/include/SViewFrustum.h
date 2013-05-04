// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VIEW_FRUSTUM_H_INCLUDED__
#define __S_VIEW_FRUSTUM_H_INCLUDED__

#include "plane3d.h"
#include "vector3d.h"
#include "line3d.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "IVideoDriver.h"

namespace irr
{
namespace scene
{

	//! Defines the view frustum. That's the space visible by the camera.
	/** The view frustum is enclosed by 6 planes. These six planes share
	eight points. A bounding box around these eight points is also stored in
	this structure.
	*/
	struct SViewFrustum
	{
		enum VFPLANES
		{
			//! Far plane of the frustum. That is the plane farest away from the eye.
			VF_FAR_PLANE = 0,
			//! Near plane of the frustum. That is the plane nearest to the eye.
			VF_NEAR_PLANE,
			//! Left plane of the frustum.
			VF_LEFT_PLANE,
			//! Right plane of the frustum.
			VF_RIGHT_PLANE,
			//! Bottom plane of the frustum.
			VF_BOTTOM_PLANE,
			//! Top plane of the frustum.
			VF_TOP_PLANE,

			//! Amount of planes enclosing the view frustum. Should be 6.
			VF_PLANE_COUNT
		};


		//! Default Constructor
		SViewFrustum() {}

		//! Copy Constructor
		SViewFrustum(const SViewFrustum& other);

		//! This constructor creates a view frustum based on a projection and/or view matrix.
		SViewFrustum(const core::matrix4& mat);

		//! This constructor creates a view frustum based on a projection and/or view matrix.
		inline void setFrom(const core::matrix4& mat);

		//! transforms the frustum by the matrix
		/** \param mat: Matrix by which the view frustum is transformed.*/
		void transform(const core::matrix4& mat);

		//! returns the point which is on the far left upper corner inside the the view frustum.
		core::vector3df getFarLeftUp() const;

		//! returns the point which is on the far left bottom corner inside the the view frustum.
		core::vector3df getFarLeftDown() const;

		//! returns the point which is on the far right top corner inside the the view frustum.
		core::vector3df getFarRightUp() const;

		//! returns the point which is on the far right bottom corner inside the the view frustum.
		core::vector3df getFarRightDown() const;

		//! returns the point which is on the near left upper corner inside the the view frustum.
		core::vector3df getNearLeftUp() const;

		//! returns the point which is on the near left bottom corner inside the the view frustum.
		core::vector3df getNearLeftDown() const;

		//! returns the point which is on the near right top corner inside the the view frustum.
		core::vector3df getNearRightUp() const;

		//! returns the point which is on the near right bottom corner inside the the view frustum.
		core::vector3df getNearRightDown() const;

		//! returns a bounding box enclosing the whole view frustum
		const core::aabbox3d<f32> &getBoundingBox() const;

		//! recalculates the bounding box member based on the planes
		inline void recalculateBoundingBox();

		//! get the given state's matrix based on frustum E_TRANSFORMATION_STATE
		core::matrix4& getTransform( video::E_TRANSFORMATION_STATE state);

		//! get the given state's matrix based on frustum E_TRANSFORMATION_STATE
		const core::matrix4& getTransform( video::E_TRANSFORMATION_STATE state) const;

		//! clips a line to the view frustum.
		/** \return True if the line was clipped, false if not */
		bool clipLine(core::line3d<f32>& line) const;

		//! the position of the camera
		core::vector3df cameraPosition;

		//! all planes enclosing the view frustum.
		core::plane3d<f32> planes[VF_PLANE_COUNT];

		//! bounding box around the view frustum
		core::aabbox3d<f32> boundingBox;

	private:
		//! Hold a copy of important transform matrices
		enum E_TRANSFORMATION_STATE_FRUSTUM
		{
			ETS_VIEW = 0,
			ETS_PROJECTION = 1,
			ETS_COUNT_FRUSTUM
		};

		//! Hold a copy of important transform matrices
		core::matrix4 Matrices[ETS_COUNT_FRUSTUM];
	};


	/*!
		Copy constructor ViewFrustum
	*/
	inline SViewFrustum::SViewFrustum(const SViewFrustum& other)
	{
		cameraPosition=other.cameraPosition;
		boundingBox=other.boundingBox;

		u32 i;
		for (i=0; i<VF_PLANE_COUNT; ++i)
			planes[i]=other.planes[i];

		for (i=0; i<ETS_COUNT_FRUSTUM; ++i)
			Matrices[i]=other.Matrices[i];
	}

	inline SViewFrustum::SViewFrustum(const core::matrix4& mat)
	{
		setFrom ( mat );
	}


	inline void SViewFrustum::transform(const core::matrix4& mat)
	{
		for (u32 i=0; i<VF_PLANE_COUNT; ++i)
			mat.transformPlane(planes[i]);

		mat.transformVect(cameraPosition);
		recalculateBoundingBox();
	}


	inline core::vector3df SViewFrustum::getFarLeftUp() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_TOP_PLANE],
			planes[scene::SViewFrustum::VF_LEFT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getFarLeftDown() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_BOTTOM_PLANE],
			planes[scene::SViewFrustum::VF_LEFT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getFarRightUp() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_TOP_PLANE],
			planes[scene::SViewFrustum::VF_RIGHT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getFarRightDown() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_BOTTOM_PLANE],
			planes[scene::SViewFrustum::VF_RIGHT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getNearLeftUp() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_TOP_PLANE],
			planes[scene::SViewFrustum::VF_LEFT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getNearLeftDown() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_BOTTOM_PLANE],
			planes[scene::SViewFrustum::VF_LEFT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getNearRightUp() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_TOP_PLANE],
			planes[scene::SViewFrustum::VF_RIGHT_PLANE], p);

		return p;
	}

	inline core::vector3df SViewFrustum::getNearRightDown() const
	{
		core::vector3df p;
		planes[scene::SViewFrustum::VF_NEAR_PLANE].getIntersectionWithPlanes(
			planes[scene::SViewFrustum::VF_BOTTOM_PLANE],
			planes[scene::SViewFrustum::VF_RIGHT_PLANE], p);

		return p;
	}

	inline const core::aabbox3d<f32> &SViewFrustum::getBoundingBox() const
	{
		return boundingBox;
	}

	inline void SViewFrustum::recalculateBoundingBox()
	{
		boundingBox.reset ( cameraPosition );

		boundingBox.addInternalPoint(getFarLeftUp());
		boundingBox.addInternalPoint(getFarRightUp());
		boundingBox.addInternalPoint(getFarLeftDown());
		boundingBox.addInternalPoint(getFarRightDown());
	}

	//! This constructor creates a view frustum based on a projection
	//! and/or view matrix.
	inline void SViewFrustum::setFrom(const core::matrix4& mat)
	{
		// left clipping plane
		planes[VF_LEFT_PLANE].Normal.X = mat[3 ] + mat[0];
		planes[VF_LEFT_PLANE].Normal.Y = mat[7 ] + mat[4];
		planes[VF_LEFT_PLANE].Normal.Z = mat[11] + mat[8];
		planes[VF_LEFT_PLANE].D =        mat[15] + mat[12];

		// right clipping plane
		planes[VF_RIGHT_PLANE].Normal.X = mat[3 ] - mat[0];
		planes[VF_RIGHT_PLANE].Normal.Y = mat[7 ] - mat[4];
		planes[VF_RIGHT_PLANE].Normal.Z = mat[11] - mat[8];
		planes[VF_RIGHT_PLANE].D =        mat[15] - mat[12];

		// top clipping plane
		planes[VF_TOP_PLANE].Normal.X = mat[3 ] - mat[1];
		planes[VF_TOP_PLANE].Normal.Y = mat[7 ] - mat[5];
		planes[VF_TOP_PLANE].Normal.Z = mat[11] - mat[9];
		planes[VF_TOP_PLANE].D =        mat[15] - mat[13];

		// bottom clipping plane
		planes[VF_BOTTOM_PLANE].Normal.X = mat[3 ] + mat[1];
		planes[VF_BOTTOM_PLANE].Normal.Y = mat[7 ] + mat[5];
		planes[VF_BOTTOM_PLANE].Normal.Z = mat[11] + mat[9];
		planes[VF_BOTTOM_PLANE].D =        mat[15] + mat[13];

		// far clipping plane
		planes[VF_FAR_PLANE].Normal.X = mat[3 ] - mat[2];
		planes[VF_FAR_PLANE].Normal.Y = mat[7 ] - mat[6];
		planes[VF_FAR_PLANE].Normal.Z = mat[11] - mat[10];
		planes[VF_FAR_PLANE].D =        mat[15] - mat[14];

		// near clipping plane
		planes[VF_NEAR_PLANE].Normal.X = mat[2];
		planes[VF_NEAR_PLANE].Normal.Y = mat[6];
		planes[VF_NEAR_PLANE].Normal.Z = mat[10];
		planes[VF_NEAR_PLANE].D =        mat[14];

		// normalize normals
		u32 i;
		for ( i=0; i != VF_PLANE_COUNT; ++i)
		{
			const f32 len = -core::reciprocal_squareroot(
					planes[i].Normal.getLengthSQ());
			planes[i].Normal *= len;
			planes[i].D *= len;
		}

		// make bounding box
		recalculateBoundingBox();
	}

	/*!
		View Frustum depends on Projection & View Matrix
	*/
	inline core::matrix4& SViewFrustum::getTransform(video::E_TRANSFORMATION_STATE state )
	{
		u32 index = 0;
		switch ( state )
		{
			case video::ETS_PROJECTION:
				index = SViewFrustum::ETS_PROJECTION; break;
			case video::ETS_VIEW:
				index = SViewFrustum::ETS_VIEW; break;
			default:
				break;
		}
		return Matrices [ index ];
	}

	/*!
		View Frustum depends on Projection & View Matrix
	*/
	inline const core::matrix4& SViewFrustum::getTransform(video::E_TRANSFORMATION_STATE state ) const
	{
		u32 index = 0;
		switch ( state )
		{
			case video::ETS_PROJECTION:
				index = SViewFrustum::ETS_PROJECTION; break;
			case video::ETS_VIEW:
				index = SViewFrustum::ETS_VIEW; break;
			default:
				break;
		}
		return Matrices [ index ];
	}

	//! Clips a line to the frustum
	inline bool SViewFrustum::clipLine(core::line3d<f32>& line) const
	{
		bool wasClipped = false;
		for (u32 i=0; i < VF_PLANE_COUNT; ++i)
		{
			if (planes[i].classifyPointRelation(line.start) == core::ISREL3D_FRONT)
			{
				line.start = line.start.getInterpolated(line.end,
						planes[i].getKnownIntersectionWithLine(line.start, line.end));
				wasClipped = true;
			}
			if (planes[i].classifyPointRelation(line.end) == core::ISREL3D_FRONT)
			{
				line.end = line.start.getInterpolated(line.end,
						planes[i].getKnownIntersectionWithLine(line.start, line.end));
				wasClipped = true;
			}
		}
		return wasClipped;
	}


} // end namespace scene
} // end namespace irr

#endif

