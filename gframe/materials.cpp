#include "materials.h"

namespace ygo {

Materials matManager;

inline void SetS3DVertex(irr::video::S3DVertex* v, irr::f32 x1, irr::f32 y1, irr::f32 x2, irr::f32 y2, irr::f32 z, irr::f32 nz, irr::f32 tu1, irr::f32 tv1, irr::f32 tu2, irr::f32 tv2) {
	v[0] = irr::video::S3DVertex(x1, y1, z, 0, 0, nz, irr::video::SColor(255, 255, 255, 255), tu1, tv1);
	v[1] = irr::video::S3DVertex(x2, y1, z, 0, 0, nz, irr::video::SColor(255, 255, 255, 255), tu2, tv1);
	v[2] = irr::video::S3DVertex(x1, y2, z, 0, 0, nz, irr::video::SColor(255, 255, 255, 255), tu1, tv2);
	v[3] = irr::video::S3DVertex(x2, y2, z, 0, 0, nz, irr::video::SColor(255, 255, 255, 255), tu2, tv2);
}

Materials::Materials() {
	SetS3DVertex(vCardFront, -0.35f, -0.5f, 0.35f, 0.5f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vCardOutline, -0.375f, -0.54f, 0.37f, 0.54f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vCardOutliner, 0.37f, -0.54f, -0.375f, 0.54f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vCardBack, 0.35f, -0.5f, -0.35f, 0.5f, 0, -1, 0, 0, 1, 1);
	SetS3DVertex(vSymbol, -0.35f, -0.35f, 0.35f, 0.35f, 0.01f, 1, 0, 0, 1, 1);
	SetS3DVertex(vNegate, -0.25f, -0.28f, 0.25f, 0.22f, 0.01f, 1, 0, 0, 1, 1);
	SetS3DVertex(vChainNum, -0.35f, -0.35f, 0.35f, 0.35f, 0, 1, 0, 0, 0.19375f, 0.2421875f);
	SetS3DVertex(vActivate, -0.5f, -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vField, -1.0f, -4.0f, 9.0f, 4.0f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vFieldSpell, 1.2f, -3.2f, 6.7f, 3.2f, 0, 1, 0, 0, 1, 1);
	SetS3DVertex(vFieldSpell1, 1.2f, 0.8f, 6.7f, 3.2f, 0, 1, 0, 0.2f, 1, 0.63636f);
	SetS3DVertex(vFieldSpell2, 1.2f, -3.2f, 6.7f, -0.8f, 0, 1, 1, 0.63636f, 0, 0.2f);

	iRectangle[0] = 0;
	iRectangle[1] = 1;
	iRectangle[2] = 2;
	iRectangle[3] = 2;
	iRectangle[4] = 1;
	iRectangle[5] = 3;

	SetS3DVertex(vFieldDeck[0], 6.9f, 2.7f, 7.7f, 3.9f, 0, 1, 0, 0, 0, 0);
	//grave
	SetS3DVertex(vFieldGrave[0][0], 6.9f, 0.1f, 7.7f, 1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldGrave[0][1], 6.9f, 1.4f, 7.7f, 2.6f, 0, 1, 0, 0, 0, 0);
	//extra
	SetS3DVertex(vFieldExtra[0], 0.2f, 2.7f, 1.0f, 3.9f, 0, 1, 0, 0, 0, 0);
	//remove
	SetS3DVertex(vFieldRemove[0][0], 7.9f, 0.1f, 8.7f, 1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldRemove[0][1], 6.9f, 0.1f, 7.7f, 1.3f, 0, 1, 0, 0, 0, 0);
	for(int i = 0; i < 5; ++i)
		SetS3DVertex(vFieldMzone[0][i], 1.2f + i * 1.1f, 0.8f, 2.3f + i * 1.1f, 2.0f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldMzone[0][5], 2.3f, -0.6f, 3.4f, 0.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldMzone[0][6], 4.5f, -0.6f, 5.6f, 0.6f, 0, 1, 0, 0, 0, 0);
	for (int i = 0; i < 5; ++i) {
		SetS3DVertex(vFieldSzone[0][i][0], 1.2f + i * 1.1f, 2.0f, 2.3f + i * 1.1f, 3.2f, 0, 1, 0, 0, 0, 0);
		SetS3DVertex(vFieldSzone[0][i][1], 1.2f + i * 1.1f, 2.0f, 2.3f + i * 1.1f, 3.2f, 0, 1, 0, 0, 0, 0);
	}
	//field
	SetS3DVertex(vFieldSzone[0][5][0], 0.2f, 0.1f, 1.0f, 1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[0][5][1], 0.2f, 1.4f, 1.0f, 2.6f, 0, 1, 0, 0, 0, 0);
	//LScale
	SetS3DVertex(vFieldSzone[0][6][0], 0.2f, 1.4f, 1.0f, 2.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[0][6][1], 0.2f, 0.1f, 1.0f, 1.3f, 0, 1, 0, 0, 0, 0);
	//RScale
	SetS3DVertex(vFieldSzone[0][7][0], 6.9f, 1.4f, 7.7f, 2.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[0][7][1], 7.9f, 0.1f, 8.7f, 1.3f, 0, 1, 0, 0, 0, 0);

	SetS3DVertex(vFieldDeck[1], 1.0f, -2.7f, 0.2f, -3.9f, 0, 1, 0, 0, 0, 0);
	//grave
	SetS3DVertex(vFieldGrave[1][0], 1.0f, -0.1f, 0.2f, -1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldGrave[1][1], 1.0f, -1.4f, 0.2f, -2.6f, 0, 1, 0, 0, 0, 0);
	//extra
	SetS3DVertex(vFieldExtra[1], 7.7f, -2.7f, 6.9f, -3.9f, 0, 1, 0, 0, 0, 0);
	//remove
	SetS3DVertex(vFieldRemove[1][0], 0.0f, -0.1f, -0.8f, -1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldRemove[1][1], 1.0f, -0.1f, 0.2f, -1.3f, 0, 1, 0, 0, 0, 0);
	for(int i = 0; i < 5; ++i)
		SetS3DVertex(vFieldMzone[1][i], 6.7f - i * 1.1f, -0.8f, 5.6f - i * 1.1f, -2.0f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldMzone[1][5], 5.6f, 0.6f, 4.5f, -0.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldMzone[1][6], 3.4f, 0.6f, 2.3f, -0.6f, 0, 1, 0, 0, 0, 0);
	for (int i = 0; i < 5; ++i) {
		SetS3DVertex(vFieldSzone[1][i][0], 6.7f - i * 1.1f, -2.0f, 5.6f - i * 1.1f, -3.2f, 0, 1, 0, 0, 0, 0);
		SetS3DVertex(vFieldSzone[1][i][1], 6.7f - i * 1.1f, -2.0f, 5.6f - i * 1.1f, -3.2f, 0, 1, 0, 0, 0, 0);
	}
	//field
	SetS3DVertex(vFieldSzone[1][5][0], 7.7f, -0.1f, 6.9f, -1.3f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[1][5][1], 7.7f, -1.4f, 6.9f, -2.6f, 0, 1, 0, 0, 0, 0);
	//LScale
	SetS3DVertex(vFieldSzone[1][6][0], 7.7f, -1.4f, 6.9f, -2.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[1][6][1], 7.7f, -0.1f, 6.9f, -1.3f, 0, 1, 0, 0, 0, 0);
	//RScale
	SetS3DVertex(vFieldSzone[1][7][0], 1.0f, -1.4f, 0.2f, -2.6f, 0, 1, 0, 0, 0, 0);
	SetS3DVertex(vFieldSzone[1][7][1], 0.0f, -0.1f, -0.8f, -1.3f, 0, 1, 0, 0, 0, 0);

	//conti_act
	vFieldContiAct[0] = irr::core::vector3df(3.5f, -0.6f, 0.0f);
	vFieldContiAct[1] = irr::core::vector3df(4.4f, -0.6f, 0.0f);
	vFieldContiAct[2] = irr::core::vector3df(3.5f, 0.6f, 0.0f);
	vFieldContiAct[3] = irr::core::vector3df(4.4f, 0.6f, 0.0f);


	for(int i = 0; i < 40; ++i)
		iArrow[i] = i;

	mCard.AmbientColor = 0xffffffff;
	mCard.DiffuseColor = 0xff000000;
	mCard.ColorMaterial = irr::video::ECM_NONE;
	mCard.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mCard.MaterialTypeParam = pack_textureBlendFunc(irr::video::EBF_SRC_ALPHA, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_VERTEX_COLOR);
	mTexture.AmbientColor = 0xffffffff;
	mTexture.DiffuseColor = 0xff000000;
	mTexture.ColorMaterial = irr::video::ECM_NONE;
	mTexture.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	mBackLine.ColorMaterial = irr::video::ECM_NONE;
	mBackLine.AmbientColor = 0xffffffff;
	mBackLine.DiffuseColor = 0xc0000000;
	mBackLine.AntiAliasing = irr::video::EAAM_FULL_BASIC;
	mBackLine.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mBackLine.MaterialTypeParam = pack_textureBlendFunc(irr::video::EBF_SRC_ALPHA, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_VERTEX_COLOR);
	mBackLine.Thickness = 2;
	mSelField.ColorMaterial = irr::video::ECM_NONE;
	mSelField.AmbientColor = 0xffffffff;
	mSelField.DiffuseColor = 0xff000000;
	mSelField.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mSelField.MaterialTypeParam = pack_textureBlendFunc(irr::video::EBF_SRC_ALPHA, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_VERTEX_COLOR);
	mOutLine.ColorMaterial = irr::video::ECM_AMBIENT;
	mOutLine.DiffuseColor = 0xff000000;
	mOutLine.Thickness = 2;
	mTRTexture = mTexture;
	mTRTexture.AmbientColor = 0xffffff00;
	mATK.ColorMaterial = irr::video::ECM_AMBIENT;
	mATK.DiffuseColor = 0x80000000;
	mATK.setFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	mATK.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mATK.MaterialTypeParam = pack_textureBlendFunc(irr::video::EBF_SRC_ALPHA, irr::video::EBF_ONE_MINUS_SRC_ALPHA, irr::video::EMFN_MODULATE_1X, irr::video::EAS_VERTEX_COLOR);
}
void Materials::GenArrow(float y) {
	float ay = 1.0f;
	for (int i = 0; i < 19; ++i) {
		vArrow[i * 2] = irr::video::S3DVertex(irr::core::vector3df(0.1f, ay * y, -2.0f * (ay * ay - 1.0f)), irr::core::vector3df(0, ay * y, 1), 0xc000ff00, irr::core::vector2df(0, 0));
		vArrow[i * 2 + 1] = irr::video::S3DVertex(irr::core::vector3df(-0.1f, ay * y, -2.0f * (ay * ay - 1.0f)), irr::core::vector3df(0, ay * y, 1), 0xc000ff00, irr::core::vector2df(0, 0));
		ay -= 0.1f;
	}
	vArrow[36].Pos.X = 0.2f;
	vArrow[36].Pos.Y = vArrow[34].Pos.Y - 0.01f;
	vArrow[36].Pos.Z = vArrow[34].Pos.Z - 0.01f;
	vArrow[37].Pos.X = -0.2f;
	vArrow[37].Pos.Y = vArrow[35].Pos.Y - 0.01f;
	vArrow[37].Pos.Z = vArrow[35].Pos.Z - 0.01f;
	vArrow[38] = irr::video::S3DVertex(irr::core::vector3df(0.0f, -1.0f * y, 0.0f), irr::core::vector3df(0.0f, -1.0f, -1.0f), 0xc0ffffff, irr::core::vector2df(0, 0));
	vArrow[39] = vArrow[38];
}

}
