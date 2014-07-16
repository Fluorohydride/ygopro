#ifndef _DRAWING_OBJECT_H_
#define _DRAWING_OBJECT_H_

namespace ygopro
{

	template<typename T>
	struct vector2d {
	public:
		T x;
		T y;
		T& operator + (const T& op) {
			x += op.x;
			y += op.y;
		}
        
		T& operator - (const T& op) {
			x -= op.x;
			y -= op.y;
		}
	};
    
	template<typename T>
	struct vector3d {
	public:
		T x;
		T y;
		T z;
		T& operator + (const T& op) {
			x += op.x;
			y += op.y;
			z += op.z;
		}
        
		T& operator - (const T& op) {
			x -= op.x;
			y -= op.y;
			y -= op.z;
		}
	};

}

#endif
