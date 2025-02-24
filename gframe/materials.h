#ifndef MATERIALS_H
#define MATERIALS_H

#include <irrlicht.h>

namespace ygo {

class Materials {
public:
	Materials();
	void GenArrow(float y);
	
	irr::video::S3DVertex vCardFront[4];
	irr::video::S3DVertex vCardOutline[4];
	irr::video::S3DVertex vCardOutliner[4];
	irr::video::S3DVertex vCardBack[4];
	irr::video::S3DVertex vSymbol[4];
	irr::video::S3DVertex vNegate[4];
	irr::video::S3DVertex vChainNum[4];
	irr::video::S3DVertex vActivate[4];
	irr::video::S3DVertex vField[4];
	irr::video::S3DVertex vFieldSpell[4];
	irr::video::S3DVertex vFieldSpell1[4];
	irr::video::S3DVertex vFieldSpell2[4];
	//irr::video::S3DVertex vBackLine[76];
	irr::video::S3DVertex vFieldDeck[2][4];
	irr::video::S3DVertex vFieldGrave[2][2][4];    //[player][rule], rule = 0: dule_rule <= 3, 1: dule_rule >= 4
	irr::video::S3DVertex vFieldExtra[2][4];
	irr::video::S3DVertex vFieldRemove[2][2][4];   //[player][rule]
	irr::video::S3DVertex vFieldMzone[2][7][4];    //[player][sequence]
	irr::video::S3DVertex vFieldSzone[2][8][2][4]; //[player][sequence][rule]
	irr::core::vector3df vFieldContiAct[4];
	irr::video::S3DVertex vArrow[40];
	irr::video::SColor c2d[4];
	irr::u16 iRectangle[6];
	//irr::u16 iBackLine[116];
	irr::u16 iArrow[40];
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

#endif //MATERIALS_H
