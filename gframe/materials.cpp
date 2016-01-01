#include "materials.h"

namespace ygo {

Materials matManager;

Materials::Materials() {
	vCardFront[0] = S3DVertex(vector3df(-0.35f, -0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vCardFront[1] = S3DVertex(vector3df(0.35f, -0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vCardFront[2] = S3DVertex(vector3df(-0.35f, 0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vCardFront[3] = S3DVertex(vector3df(0.35f, 0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vCardOutline[0] = S3DVertex(vector3df(-0.375f, -0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vCardOutline[1] = S3DVertex(vector3df(0.37f, -0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vCardOutline[2] = S3DVertex(vector3df(-0.375f, 0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vCardOutline[3] = S3DVertex(vector3df(0.37f, 0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vCardOutliner[0] = S3DVertex(vector3df(0.37f, -0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vCardOutliner[1] = S3DVertex(vector3df(-0.375f, -0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vCardOutliner[2] = S3DVertex(vector3df(0.37f, 0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vCardOutliner[3] = S3DVertex(vector3df(-0.375f, 0.54f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vCardBack[0] = S3DVertex(vector3df(0.35f, -0.5f, 0), vector3df(0, 0, -1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vCardBack[1] = S3DVertex(vector3df(-0.35f, -0.5f, 0), vector3df(0, 0, -1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vCardBack[2] = S3DVertex(vector3df(0.35f, 0.5f, 0), vector3df(0, 0, -1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vCardBack[3] = S3DVertex(vector3df(-0.35f, 0.5f, 0), vector3df(0, 0, -1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vSymbol[0] = S3DVertex(vector3df(-0.35f, -0.35f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vSymbol[1] = S3DVertex(vector3df(0.35f, -0.35f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vSymbol[2] = S3DVertex(vector3df(-0.35f, 0.35f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vSymbol[3] = S3DVertex(vector3df(0.35f, 0.35f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vNegate[0] = S3DVertex(vector3df(-0.25f, -0.28f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vNegate[1] = S3DVertex(vector3df(0.25f, -0.28f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vNegate[2] = S3DVertex(vector3df(-0.25f, 0.22f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vNegate[3] = S3DVertex(vector3df(0.25f, 0.22f, 0.01f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vChainNum[0] = S3DVertex(vector3df(-0.35f, -0.35f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vChainNum[1] = S3DVertex(vector3df(0.35f, -0.35f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0.19375f, 0));
	vChainNum[2] = S3DVertex(vector3df(-0.35f, 0.35f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0.2421875f));
	vChainNum[3] = S3DVertex(vector3df(0.35f, 0.35f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0.19375f, 0.2421875f));
	vActivate[0] = S3DVertex(vector3df(-0.5f, -0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vActivate[1] = S3DVertex(vector3df(0.5f, -0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vActivate[2] = S3DVertex(vector3df(-0.5f, 0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vActivate[3] = S3DVertex(vector3df(0.5f, 0.5f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vField[0] = S3DVertex(vector3df(-1.0f, -4.0f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vField[1] = S3DVertex(vector3df(9.0f, -4.0f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vField[2] = S3DVertex(vector3df(-1.0f, 4.0f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vField[3] = S3DVertex(vector3df(9.0f, 4.0f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vFieldSpell[0] = S3DVertex(vector3df(1.2f, -3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFieldSpell[1] = S3DVertex(vector3df(6.7f, -3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vFieldSpell[2] = S3DVertex(vector3df(1.2f, 3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vFieldSpell[3] = S3DVertex(vector3df(6.7f, 3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vFieldSpell1[0] = S3DVertex(vector3df(1.2f, 0.8f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFieldSpell1[1] = S3DVertex(vector3df(6.7f, 0.8f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vFieldSpell1[2] = S3DVertex(vector3df(1.2f, 3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vFieldSpell1[3] = S3DVertex(vector3df(6.7f, 3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
	vFieldSpell2[0] = S3DVertex(vector3df(1.2f, -3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFieldSpell2[1] = S3DVertex(vector3df(6.7f, -3.2f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 0));
	vFieldSpell2[2] = S3DVertex(vector3df(1.2f, -0.8f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 1));
	vFieldSpell2[3] = S3DVertex(vector3df(6.7f, -0.8f, 0), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(1, 1));
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

	iRectangle[0] = 0;
	iRectangle[1] = 1;
	iRectangle[2] = 2;
	iRectangle[3] = 2;
	iRectangle[4] = 1;
	iRectangle[5] = 3;

	//deck0
	vFields[0] = S3DVertex(vector3df(6.9f, 2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[1] = S3DVertex(vector3df(7.7f, 2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[2] = S3DVertex(vector3df(6.9f, 3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[3] = S3DVertex(vector3df(7.7f, 3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//grave0
	vFields[4] = S3DVertex(vector3df(6.9f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[5] = S3DVertex(vector3df(7.7f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[6] = S3DVertex(vector3df(6.9f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[7] = S3DVertex(vector3df(7.7f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//extra0
	vFields[8] = S3DVertex(vector3df(0.2f, 2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[9] = S3DVertex(vector3df(1.0f, 2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[10] = S3DVertex(vector3df(0.2f, 3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[11] = S3DVertex(vector3df(1.0f, 3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//remove0
	vFields[12] = S3DVertex(vector3df(7.9f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[13] = S3DVertex(vector3df(8.7f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[14] = S3DVertex(vector3df(7.9f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[15] = S3DVertex(vector3df(8.7f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//mzone0
	vFields[16] = S3DVertex(vector3df(1.2f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[17] = S3DVertex(vector3df(2.3f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[18] = S3DVertex(vector3df(1.2f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[19] = S3DVertex(vector3df(2.3f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[20] = S3DVertex(vector3df(2.3f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[21] = S3DVertex(vector3df(3.4f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[22] = S3DVertex(vector3df(2.3f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[23] = S3DVertex(vector3df(3.4f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[24] = S3DVertex(vector3df(3.4f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[25] = S3DVertex(vector3df(4.5f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[26] = S3DVertex(vector3df(3.4f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[27] = S3DVertex(vector3df(4.5f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[28] = S3DVertex(vector3df(4.5f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[29] = S3DVertex(vector3df(5.6f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[30] = S3DVertex(vector3df(4.5f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[31] = S3DVertex(vector3df(5.6f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[32] = S3DVertex(vector3df(5.6f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[33] = S3DVertex(vector3df(6.7f, 0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[34] = S3DVertex(vector3df(5.6f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[35] = S3DVertex(vector3df(6.7f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//szone0
	vFields[36] = S3DVertex(vector3df(1.2f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[37] = S3DVertex(vector3df(2.3f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[38] = S3DVertex(vector3df(1.2f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[39] = S3DVertex(vector3df(2.3f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[40] = S3DVertex(vector3df(2.3f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[41] = S3DVertex(vector3df(3.4f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[42] = S3DVertex(vector3df(2.3f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[43] = S3DVertex(vector3df(3.4f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[44] = S3DVertex(vector3df(3.4f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[45] = S3DVertex(vector3df(4.5f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[46] = S3DVertex(vector3df(3.4f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[47] = S3DVertex(vector3df(4.5f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[48] = S3DVertex(vector3df(4.5f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[49] = S3DVertex(vector3df(5.6f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[50] = S3DVertex(vector3df(4.5f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[51] = S3DVertex(vector3df(5.6f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[52] = S3DVertex(vector3df(5.6f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[53] = S3DVertex(vector3df(6.7f, 2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[54] = S3DVertex(vector3df(5.6f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[55] = S3DVertex(vector3df(6.7f, 3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[56] = S3DVertex(vector3df(0.2f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//field
	vFields[57] = S3DVertex(vector3df(1.0f, 0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[58] = S3DVertex(vector3df(0.2f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[59] = S3DVertex(vector3df(1.0f, 1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[60] = S3DVertex(vector3df(0.2f, 1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//LScale
	vFields[61] = S3DVertex(vector3df(1.0f, 1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[62] = S3DVertex(vector3df(0.2f, 2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[63] = S3DVertex(vector3df(1.0f, 2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[64] = S3DVertex(vector3df(6.9f, 1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//RScale
	vFields[65] = S3DVertex(vector3df(7.7f, 1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[66] = S3DVertex(vector3df(6.9f, 2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[67] = S3DVertex(vector3df(7.7f, 2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//deck1
	vFields[68] = S3DVertex(vector3df(1.0f, -2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[69] = S3DVertex(vector3df(0.2f, -2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[70] = S3DVertex(vector3df(1.0f, -3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[71] = S3DVertex(vector3df(0.2f, -3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//grave1
	vFields[72] = S3DVertex(vector3df(1.0f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[73] = S3DVertex(vector3df(0.2f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[74] = S3DVertex(vector3df(1.0f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[75] = S3DVertex(vector3df(0.2f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//extra1
	vFields[76] = S3DVertex(vector3df(7.7f, -2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[77] = S3DVertex(vector3df(6.9f, -2.7f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[78] = S3DVertex(vector3df(7.7f, -3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[79] = S3DVertex(vector3df(6.9f, -3.9f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//remove1
	vFields[80] = S3DVertex(vector3df(0.0f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[81] = S3DVertex(vector3df(-0.8f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[82] = S3DVertex(vector3df(0.0f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[83] = S3DVertex(vector3df(-0.8f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//mzone1
	vFields[84] = S3DVertex(vector3df(6.7f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[85] = S3DVertex(vector3df(5.6f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[86] = S3DVertex(vector3df(6.7f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[87] = S3DVertex(vector3df(5.6f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[88] = S3DVertex(vector3df(5.6f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[89] = S3DVertex(vector3df(4.5f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[90] = S3DVertex(vector3df(5.6f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[91] = S3DVertex(vector3df(4.5f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[92] = S3DVertex(vector3df(4.5f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[93] = S3DVertex(vector3df(3.4f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[94] = S3DVertex(vector3df(4.5f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[95] = S3DVertex(vector3df(3.4f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[96] = S3DVertex(vector3df(3.4f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[97] = S3DVertex(vector3df(2.3f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[98] = S3DVertex(vector3df(3.4f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[99] = S3DVertex(vector3df(2.3f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[100] = S3DVertex(vector3df(2.3f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[101] = S3DVertex(vector3df(1.2f, -0.8f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[102] = S3DVertex(vector3df(2.3f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[103] = S3DVertex(vector3df(1.2f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	//szone1
	vFields[104] = S3DVertex(vector3df(6.7f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[105] = S3DVertex(vector3df(5.6f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[106] = S3DVertex(vector3df(6.7f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[107] = S3DVertex(vector3df(5.6f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[108] = S3DVertex(vector3df(5.6f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[109] = S3DVertex(vector3df(4.5f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[110] = S3DVertex(vector3df(5.6f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[111] = S3DVertex(vector3df(4.5f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[112] = S3DVertex(vector3df(4.5f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[113] = S3DVertex(vector3df(3.4f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[114] = S3DVertex(vector3df(4.5f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[115] = S3DVertex(vector3df(3.4f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[116] = S3DVertex(vector3df(3.4f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[117] = S3DVertex(vector3df(2.3f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[118] = S3DVertex(vector3df(3.4f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[119] = S3DVertex(vector3df(2.3f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[120] = S3DVertex(vector3df(2.3f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[121] = S3DVertex(vector3df(1.2f, -2.0f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[122] = S3DVertex(vector3df(2.3f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[123] = S3DVertex(vector3df(1.2f, -3.2f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[124] = S3DVertex(vector3df(7.7f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//field
	vFields[125] = S3DVertex(vector3df(6.9f, -0.1f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[126] = S3DVertex(vector3df(7.7f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[127] = S3DVertex(vector3df(6.9f, -1.3f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[128] = S3DVertex(vector3df(7.7f, -1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//lscale
	vFields[129] = S3DVertex(vector3df(6.9f, -1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[130] = S3DVertex(vector3df(7.7f, -2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[131] = S3DVertex(vector3df(6.9f, -2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[132] = S3DVertex(vector3df(1.0f, -1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));//rscale
	vFields[133] = S3DVertex(vector3df(0.2f, -1.4f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[134] = S3DVertex(vector3df(1.0f, -2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));
	vFields[135] = S3DVertex(vector3df(0.2f, -2.6f, 0.0f), vector3df(0, 0, 1), SColor(255, 255, 255, 255), vector2df(0, 0));


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
