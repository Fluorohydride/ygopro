#include "materials.h"

namespace ygo {

Materials matManager;

inline void SetS3DVertex(S3DVertex* v, f32 x1, f32 y1, f32 x2, f32 y2, f32 z, f32 nz, f32 tu1, f32 tv1, f32 tu2, f32 tv2) {
	v[0] = S3DVertex(x1, y1, z, 0, 0, nz, SColor(255, 255, 255, 255), tu1, tv1);
	v[1] = S3DVertex(x2, y1, z, 0, 0, nz, SColor(255, 255, 255, 255), tu2, tv1);
	v[2] = S3DVertex(x1, y2, z, 0, 0, nz, SColor(255, 255, 255, 255), tu1, tv2);
	v[3] = S3DVertex(x2, y2, z, 0, 0, nz, SColor(255, 255, 255, 255), tu2, tv2);
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

	/*
	//background grids
	for (int i = 0; i < 6; ++i) {
		vBackLine[i * 6 + 0] = S3DVertex(vector3df(1.2f + i * 1.1f, 0.5f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
		vBackLine[i * 6 + 1] = S3DVertex(vector3df(1.2f + i * 1.1f, -0.5f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
		vBackLine[i * 6 + 2] = S3DVertex(vector3df(1.2f + i * 1.1f, 1.7f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
		vBackLine[i * 6 + 3] = S3DVertex(vector3df(1.2f + i * 1.1f, -1.7f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
		vBackLine[i * 6 + 4] = S3DVertex(vector3df(1.2f + i * 1.1f, 2.9f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
		vBackLine[i * 6 + 5] = S3DVertex(vector3df(1.2f + i * 1.1f, -2.9f, -0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	}
	for(int i = 0; i < 6; ++i) {
		iBackLine[i * 4 + 0] = i * 6 + 0;
		iBackLine[i * 4 + 1] = i * 6 + 4;
		iBackLine[i * 4 + 2] = i * 6 + 1;
		iBackLine[i * 4 + 3] = i * 6 + 5;
		iBackLine[i * 2 + 24] = i;
		iBackLine[i * 2 + 25] = 30 + i;
	}
	//extra0
	vBackLine[36] = S3DVertex(vector3df(0.2f, 2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[37] = S3DVertex(vector3df(1.0f, 2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[38] = S3DVertex(vector3df(0.2f, 3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[39] = S3DVertex(vector3df(1.0f, 3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[36] = 36;
	iBackLine[37] = 37;
	iBackLine[38] = 36;
	iBackLine[39] = 38;
	iBackLine[40] = 37;
	iBackLine[41] = 39;
	iBackLine[42] = 38;
	iBackLine[43] = 39;
	//field0
	vBackLine[40] = S3DVertex(vector3df(0.2f, 1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[41] = S3DVertex(vector3df(1.0f, 1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[42] = S3DVertex(vector3df(0.2f, 2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[43] = S3DVertex(vector3df(1.0f, 2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[44] = 40;
	iBackLine[45] = 41;
	iBackLine[46] = 40;
	iBackLine[47] = 42;
	iBackLine[48] = 41;
	iBackLine[49] = 43;
	iBackLine[50] = 42;
	iBackLine[51] = 43;
	//deck0
	vBackLine[44] = S3DVertex(vector3df(6.9f, 2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[45] = S3DVertex(vector3df(7.7f, 2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[46] = S3DVertex(vector3df(6.9f, 3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[47] = S3DVertex(vector3df(7.7f, 3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[52] = 44;
	iBackLine[53] = 45;
	iBackLine[54] = 44;
	iBackLine[55] = 46;
	iBackLine[56] = 45;
	iBackLine[57] = 47;
	iBackLine[58] = 46;
	iBackLine[59] = 47;
	//grave0
	vBackLine[48] = S3DVertex(vector3df(6.9f, 1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[49] = S3DVertex(vector3df(7.7f, 1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[50] = S3DVertex(vector3df(6.9f, 2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[51] = S3DVertex(vector3df(7.7f, 2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[60] = 48;
	iBackLine[61] = 49;
	iBackLine[62] = 48;
	iBackLine[63] = 50;
	iBackLine[64] = 49;
	iBackLine[65] = 51;
	iBackLine[66] = 50;
	iBackLine[67] = 51;
	//remove0
	vBackLine[52] = S3DVertex(vector3df(6.9f, -0.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[53] = S3DVertex(vector3df(7.7f, -0.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[54] = S3DVertex(vector3df(6.9f, 1.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[55] = S3DVertex(vector3df(7.7f, 1.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[68] = 52;
	iBackLine[69] = 53;
	iBackLine[70] = 52;
	iBackLine[71] = 54;
	iBackLine[72] = 53;
	iBackLine[73] = 55;
	iBackLine[74] = 54;
	iBackLine[75] = 55;
	//extra1
	vBackLine[56] = S3DVertex(vector3df(6.9f, -2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[57] = S3DVertex(vector3df(7.7f, -2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[58] = S3DVertex(vector3df(6.9f, -3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[59] = S3DVertex(vector3df(7.7f, -3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[76] = 56;
	iBackLine[77] = 57;
	iBackLine[78] = 56;
	iBackLine[79] = 58;
	iBackLine[80] = 57;
	iBackLine[81] = 59;
	iBackLine[82] = 58;
	iBackLine[83] = 59;
	//field1
	vBackLine[60] = S3DVertex(vector3df(6.9f, -1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[61] = S3DVertex(vector3df(7.7f, -1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[62] = S3DVertex(vector3df(6.9f, -2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[63] = S3DVertex(vector3df(7.7f, -2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[84] = 60;
	iBackLine[85] = 61;
	iBackLine[86] = 60;
	iBackLine[87] = 62;
	iBackLine[88] = 61;
	iBackLine[89] = 63;
	iBackLine[90] = 62;
	iBackLine[91] = 63;
	//deck1
	vBackLine[64] = S3DVertex(vector3df(0.2f, -2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[65] = S3DVertex(vector3df(1.0f, -2.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[66] = S3DVertex(vector3df(0.2f, -3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[67] = S3DVertex(vector3df(1.0f, -3.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[92] = 64;
	iBackLine[93] = 65;
	iBackLine[94] = 64;
	iBackLine[95] = 66;
	iBackLine[96] = 65;
	iBackLine[97] = 67;
	iBackLine[98] = 66;
	iBackLine[99] = 67;
	//grave1
	vBackLine[68] = S3DVertex(vector3df(0.2f, -1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[69] = S3DVertex(vector3df(1.0f, -1.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[70] = S3DVertex(vector3df(0.2f, -2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[71] = S3DVertex(vector3df(1.0f, -2.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[100] = 68;
	iBackLine[101] = 69;
	iBackLine[102] = 68;
	iBackLine[103] = 70;
	iBackLine[104] = 69;
	iBackLine[105] = 71;
	iBackLine[106] = 70;
	iBackLine[107] = 71;
	//remove1
	vBackLine[72] = S3DVertex(vector3df(0.2f, 0.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[73] = S3DVertex(vector3df(1.0f, 0.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[74] = S3DVertex(vector3df(0.2f, -1.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vBackLine[75] = S3DVertex(vector3df(1.0f, -1.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	iBackLine[108] = 72;
	iBackLine[109] = 73;
	iBackLine[110] = 72;
	iBackLine[111] = 74;
	iBackLine[112] = 73;
	iBackLine[113] = 75;
	iBackLine[114] = 74;
	iBackLine[115] = 75;
	*/

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
	vFieldContiAct[0] = vector3df(3.5f, -0.6f, 0.0f);
	vFieldContiAct[1] = vector3df(4.4f, -0.6f, 0.0f);
	vFieldContiAct[2] = vector3df(3.5f, 0.6f, 0.0f);
	vFieldContiAct[3] = vector3df(4.4f, 0.6f, 0.0f);


	for(int i = 0; i < 40; ++i)
		iArrow[i] = i;

	mCard.AmbientColor = 0xffffffff;
	mCard.DiffuseColor = 0xff000000;
	mCard.ColorMaterial = irr::video::ECM_NONE;
	mCard.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mCard.MaterialTypeParam = pack_textureBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA, EMFN_MODULATE_1X, EAS_VERTEX_COLOR);
	mTexture.AmbientColor = 0xffffffff;
	mTexture.DiffuseColor = 0xff000000;
	mTexture.ColorMaterial = irr::video::ECM_NONE;
	mTexture.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	mBackLine.ColorMaterial = irr::video::ECM_NONE;
	mBackLine.AmbientColor = 0xffffffff;
	mBackLine.DiffuseColor = 0xc0000000;
	mBackLine.AntiAliasing = EAAM_FULL_BASIC;
	mBackLine.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mBackLine.MaterialTypeParam = pack_textureBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA, EMFN_MODULATE_1X, EAS_VERTEX_COLOR);
	mBackLine.Thickness = 2;
	mSelField.ColorMaterial = irr::video::ECM_NONE;
	mSelField.AmbientColor = 0xffffffff;
	mSelField.DiffuseColor = 0xff000000;
	mSelField.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mSelField.MaterialTypeParam = pack_textureBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA, EMFN_MODULATE_1X, EAS_VERTEX_COLOR);
	mOutLine.ColorMaterial = irr::video::ECM_AMBIENT;
	mOutLine.DiffuseColor = 0xff000000;
	mOutLine.Thickness = 2;
	mTRTexture = mTexture;
	mTRTexture.AmbientColor = 0xffffff00;
	mATK.ColorMaterial = irr::video::ECM_AMBIENT;
	mATK.DiffuseColor = 0x80000000;
	mATK.setFlag(EMF_BACK_FACE_CULLING, FALSE);
	mATK.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;
	mATK.MaterialTypeParam = pack_textureBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA, EMFN_MODULATE_1X, EAS_VERTEX_COLOR);
}
void Materials::GenArrow(float y) {
	float ay = 1.0f;
	for (int i = 0; i < 19; ++i) {
		vArrow[i * 2] = S3DVertex(vector3df(0.1f, ay * y, -2.0f * (ay * ay - 1.0f)), vector3df(0, ay * y, 1), 0xc000ff00, vector2df(0, 0));
		vArrow[i * 2 + 1] = S3DVertex(vector3df(-0.1f, ay * y, -2.0f * (ay * ay - 1.0f)), vector3df(0, ay * y, 1), 0xc000ff00, vector2df(0, 0));
		ay -= 0.1f;
	}
	vArrow[36].Pos.X = 0.2f;
	vArrow[36].Pos.Y = vArrow[34].Pos.Y - 0.01f;
	vArrow[36].Pos.Z = vArrow[34].Pos.Z - 0.01f;
	vArrow[37].Pos.X = -0.2f;
	vArrow[37].Pos.Y = vArrow[35].Pos.Y - 0.01f;
	vArrow[37].Pos.Z = vArrow[35].Pos.Z - 0.01f;
	vArrow[38] = S3DVertex(vector3df(0.0f, -1.0f * y, 0.0f), vector3df(0.0f, -1.0f, -1.0f), 0xc0ffffff, vector2df(0, 0));
	vArrow[39] = vArrow[38];
}

}
