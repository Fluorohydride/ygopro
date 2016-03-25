#include "config.h"

namespace ygo {

class Materials {
public:
	Materials();
	void GenArrow(float y);
	
	S3DVertex vCardFront[4];
	S3DVertex vCardOutline[4];
	S3DVertex vCardOutliner[4];
	S3DVertex vCardBack[4];
	S3DVertex vSymbol[4];
	S3DVertex vNegate[4];
	S3DVertex vChainNum[4];
	S3DVertex vActivate[4];
	S3DVertex vField[4];
	S3DVertex vFieldSpell[4];
	S3DVertex vFieldSpell1[4];
	S3DVertex vFieldSpell2[4];
	S3DVertex vBackLine[76];
	S3DVertex vFields[136];
	S3DVertex vArrow[40];
	SColor c2d[4];
	u16 iRectangle[6];
	u16 iBackLine[116];
	u16 iArrow[40];
	irr::video::SMaterial mCard;
	irr::video::SMaterial mTexture;
	irr::video::SMaterial mBackLine;
	irr::video::SMaterial mOutLine;
	irr::video::SMaterial mSelField;
	irr::video::SMaterial mTRTexture;
	irr::video::SMaterial mATK;
};

extern Materials matManager;

}
