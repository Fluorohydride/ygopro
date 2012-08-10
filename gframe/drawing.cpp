#include "game.h"
#include "materials.h"
#include "image_manager.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "../ocgcore/field.h"

namespace ygo {

void Game::DrawSelectionLine(irr::video::S3DVertex* vec, bool strip, int width, float* cv) {
	if(!gameConf.use_d3d) {
		float origin[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glLineWidth(width);
		glLineStipple(1, linePattern);
		if(strip)
			glEnable(GL_LINE_STIPPLE);
		glDisable(GL_TEXTURE_2D);
		glMaterialfv(GL_FRONT, GL_AMBIENT, cv);
		glBegin(GL_LINE_LOOP);
		glVertex3fv((float*)&vec[0].Pos);
		glVertex3fv((float*)&vec[1].Pos);
		glVertex3fv((float*)&vec[3].Pos);
		glVertex3fv((float*)&vec[2].Pos);
		glEnd();
		glMaterialfv(GL_FRONT, GL_AMBIENT, origin);
		glDisable(GL_LINE_STIPPLE);
	} else {
		driver->setMaterial(matManager.mOutLine);
		if(strip) {
			if(linePattern < 15) {
				driver->draw3DLine(vec[0].Pos, vec[0].Pos + (vec[1].Pos - vec[0].Pos) * (linePattern + 1) / 15.0);
				driver->draw3DLine(vec[1].Pos, vec[1].Pos + (vec[3].Pos - vec[1].Pos) * (linePattern + 1) / 15.0);
				driver->draw3DLine(vec[3].Pos, vec[3].Pos + (vec[2].Pos - vec[3].Pos) * (linePattern + 1) / 15.0);
				driver->draw3DLine(vec[2].Pos, vec[2].Pos + (vec[0].Pos - vec[2].Pos) * (linePattern + 1) / 15.0);
			} else {
				driver->draw3DLine(vec[0].Pos + (vec[1].Pos - vec[0].Pos) * (linePattern - 14) / 15.0, vec[1].Pos);
				driver->draw3DLine(vec[1].Pos + (vec[3].Pos - vec[1].Pos) * (linePattern - 14) / 15.0, vec[3].Pos);
				driver->draw3DLine(vec[3].Pos + (vec[2].Pos - vec[3].Pos) * (linePattern - 14) / 15.0, vec[2].Pos);
				driver->draw3DLine(vec[2].Pos + (vec[0].Pos - vec[2].Pos) * (linePattern - 14) / 15.0, vec[0].Pos);
			}
		} else {
			driver->draw3DLine(vec[0].Pos, vec[1].Pos);
			driver->draw3DLine(vec[1].Pos, vec[3].Pos);
			driver->draw3DLine(vec[3].Pos, vec[2].Pos);
			driver->draw3DLine(vec[2].Pos, vec[0].Pos);
		}
	}
}
void Game::DrawBackGround() {
	static int selFieldAlpha = 255;
	static int selFieldDAlpha = -10;
	matrix4 im = irr::core::IdentityMatrix;
	im.setTranslation(vector3df(0, 0, -0.01));
	driver->setTransform(irr::video::ETS_WORLD, im);
	//dark shade
//	matManager.mSelField.AmbientColor = 0xff000000;
//	matManager.mSelField.DiffuseColor = 0xa0000000;
//	driver->setMaterial(matManager.mSelField);
//	for(int i = 0; i < 120; i += 4)
//		driver->drawVertexPrimitiveList(&matManager.vFields[i], 4, matManager.iRectangle, 2);
//	driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
//	driver->setMaterial(matManager.mBackLine);
//	driver->drawVertexPrimitiveList(matManager.vBackLine, 76, matManager.iBackLine, 58, irr::video::EVT_STANDARD, irr::scene::EPT_LINES);
	//draw field
	driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
	matManager.mTexture.setTexture(0, imageManager.tField);
	driver->setMaterial(matManager.mTexture);
	driver->drawVertexPrimitiveList(matManager.vField, 4, matManager.iRectangle, 2);
	driver->setMaterial(matManager.mBackLine);
	//select field
	if(dInfo.curMsg == MSG_SELECT_PLACE || dInfo.curMsg == MSG_SELECT_DISFIELD) {
		float cv[4] = {0.0f, 0.0f, 1.0f, 1.0f};
		int filter = 0x1;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.selectable_field & filter) > 0)
				DrawSelectionLine(&matManager.vFields[16 + i * 4], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x100;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.selectable_field & filter) > 0)
				DrawSelectionLine(&matManager.vFields[36 + i * 4], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x10000;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.selectable_field & filter) > 0)
				DrawSelectionLine(&matManager.vFields[76 + i * 4], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x1000000;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.selectable_field & filter) > 0)
				DrawSelectionLine(&matManager.vFields[96 + i * 4], !(dField.selected_field & filter), 2, cv);
		}
	}
	//disabled field
	{
		float cv[4] = {0.0f, 0.0f, 1.0f, 1.0f};
		int filter = 0x1;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.disabled_field & filter) > 0) {
				driver->draw3DLine(matManager.vFields[16 + i * 4].Pos, matManager.vFields[16 + i * 4 + 3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFields[16 + i * 4 + 1].Pos, matManager.vFields[16 + i * 4 + 2].Pos, 0xffffffff);
			}
		}
		filter = 0x100;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.disabled_field & filter) > 0) {
				driver->draw3DLine(matManager.vFields[36 + i * 4].Pos, matManager.vFields[36 + i * 4 + 3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFields[36 + i * 4 + 1].Pos, matManager.vFields[36 + i * 4 + 2].Pos, 0xffffffff);
			}
		}
		filter = 0x10000;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.disabled_field & filter) > 0) {
				driver->draw3DLine(matManager.vFields[76 + i * 4].Pos, matManager.vFields[76 + i * 4 + 3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFields[76 + i * 4 + 1].Pos, matManager.vFields[76 + i * 4 + 2].Pos, 0xffffffff);
			}
		}
		filter = 0x1000000;
		for (int i = 0; i < 5; ++i, filter <<= 1) {
			if ((dField.disabled_field & filter) > 0) {
				driver->draw3DLine(matManager.vFields[96 + i * 4].Pos, matManager.vFields[96 + i * 4 + 3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFields[96 + i * 4 + 1].Pos, matManager.vFields[96 + i * 4 + 2].Pos, 0xffffffff);
			}
		}
	}
	//current sel
	if (dField.hovered_location != 0 && dField.hovered_location  != 2) {
		int index = 0;
		if (dField.hovered_controler == 0) {
			if (dField.hovered_location == LOCATION_DECK) index = 0;
			else if (dField.hovered_location == LOCATION_MZONE) index = 16 + dField.hovered_sequence * 4;
			else if (dField.hovered_location == LOCATION_SZONE) index = 36 + dField.hovered_sequence * 4;
			else if (dField.hovered_location == LOCATION_GRAVE) index = 4;
			else if (dField.hovered_location == LOCATION_REMOVED) index = 12;
			else if (dField.hovered_location == LOCATION_EXTRA) index = 8;
		} else {
			if (dField.hovered_location == LOCATION_DECK) index = 60;
			else if (dField.hovered_location == LOCATION_MZONE) index = 76 + dField.hovered_sequence * 4;
			else if (dField.hovered_location == LOCATION_SZONE) index = 96 + dField.hovered_sequence * 4;
			else if (dField.hovered_location == LOCATION_GRAVE) index = 64;
			else if (dField.hovered_location == LOCATION_REMOVED) index = 72;
			else if (dField.hovered_location == LOCATION_EXTRA) index = 68;
		}
		selFieldAlpha += selFieldDAlpha;
		if (selFieldAlpha <= 5) {
			selFieldAlpha = 5;
			selFieldDAlpha = 10;
		}
		if (selFieldAlpha >= 205) {
			selFieldAlpha = 205;
			selFieldDAlpha = -10;
		}
		matManager.mSelField.AmbientColor = 0xffffffff;
		matManager.mSelField.DiffuseColor = selFieldAlpha << 24;
		driver->setMaterial(matManager.mSelField);
		driver->drawVertexPrimitiveList(&matManager.vFields[index], 4, matManager.iRectangle, 2);
	}
}
void Game::DrawCards() {
	for(int p = 0; p < 2; ++p) {
		for(int i = 0; i < 5; ++i)
			if(dField.mzone[p][i])
				DrawCard(dField.mzone[p][i]);
		for(int i = 0; i < 6; ++i)
			if(dField.szone[p][i])
				DrawCard(dField.szone[p][i]);
		for(int i = 0; i < dField.deck[p].size(); ++i)
			DrawCard(dField.deck[p][i]);
		for(int i = 0; i < dField.hand[p].size(); ++i)
			DrawCard(dField.hand[p][i]);
		for(int i = 0; i < dField.grave[p].size(); ++i)
			DrawCard(dField.grave[p][i]);
		for(int i = 0; i < dField.remove[p].size(); ++i)
			DrawCard(dField.remove[p][i]);
		for(int i = 0; i < dField.extra[p].size(); ++i)
			DrawCard(dField.extra[p][i]);
	}
	for(auto cit = dField.overlay_cards.begin(); cit != dField.overlay_cards.end(); ++cit)
		DrawCard(*cit);
}
void Game::DrawCard(ClientCard* pcard) {
	driver->setTransform(irr::video::ETS_WORLD, pcard->mTransform);
	if(pcard->aniFrame) {
		if(pcard->is_moving) {
			pcard->curPos += pcard->dPos;
			pcard->curRot += pcard->dRot;
			pcard->mTransform.setTranslation(pcard->curPos);
			pcard->mTransform.setRotationRadians(pcard->curRot);
		}
		if(pcard->is_fading)
			pcard->curAlpha += pcard->dAlpha;
		pcard->aniFrame--;
		if(pcard->aniFrame == 0) {
			pcard->is_moving = false;
			pcard->is_fading = false;
		}
	}
	matManager.mCard.AmbientColor = 0xffffffff;
	matManager.mCard.DiffuseColor = (pcard->curAlpha << 24) | 0xffffff;
	matManager.mCard.setTexture(0, imageManager.GetTexture(pcard->code));
	driver->setTransform(irr::video::ETS_WORLD, pcard->mTransform);
	driver->setMaterial(matManager.mCard);
	driver->drawVertexPrimitiveList(matManager.vCardFront, 4, matManager.iRectangle, 2);
	matManager.mCard.setTexture(0, imageManager.tCover);
	driver->setMaterial(matManager.mCard);
	driver->drawVertexPrimitiveList(matManager.vCardBack, 4, matManager.iRectangle, 2);
	if(pcard->is_showequip) {
		matManager.mTexture.setTexture(0, imageManager.tEquip);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	} else if(pcard->is_showtarget) {
		matManager.mTexture.setTexture(0, imageManager.tTarget);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	} else if(pcard->is_disabled && (pcard->location & LOCATION_ONFIELD) && (pcard->position & POS_FACEUP)) {
		matManager.mTexture.setTexture(0, imageManager.tNegated);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vNegate, 4, matManager.iRectangle, 2);
	}
	if(pcard->is_selectable && (pcard->location & 0xe)) {
		float cv[4] = {1.0f, 1.0f, 0.0f, 1.0f};
		if((pcard->location == LOCATION_HAND && pcard->code) || ((pcard->location & 0xc) && (pcard->position & POS_FACEUP)))
			DrawSelectionLine(matManager.vCardOutline, !pcard->is_selected, 2, cv);
		else
			DrawSelectionLine(matManager.vCardOutliner, !pcard->is_selected, 2, cv);
	}
	if(pcard->is_highlighting) {
		float cv[4] = {0.0f, 1.0f, 1.0f, 1.0f};
		if((pcard->location == LOCATION_HAND && pcard->code) || ((pcard->location & 0xc) && (pcard->position & POS_FACEUP)))
			DrawSelectionLine(matManager.vCardOutline, true, 2, cv);
		else
			DrawSelectionLine(matManager.vCardOutliner, true, 2, cv);
	}
	if(pcard->cmdFlag & COMMAND_ATTACK) {
		matManager.mTexture.setTexture(0, imageManager.tAttack);
		driver->setMaterial(matManager.mTexture);
		irr::core::matrix4 atk;
		atk.setTranslation(pcard->curPos + vector3df(0, -atkdy / 4.0f - 0.35f, 0.05f));
		driver->setTransform(irr::video::ETS_WORLD, atk);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	}
}
void Game::DrawMisc() {
	static irr::core::vector3df act_rot(0, 0, 0);
	irr::core::matrix4 im, ic, it;
	act_rot.Z += 0.02;
	im.setRotationRadians(act_rot);
	matManager.mTexture.setTexture(0, imageManager.tAct);
	driver->setMaterial(matManager.mTexture);
	if(dField.deck_act) {
		im.setTranslation(vector3df(7.3f, 3.0f, dField.deck[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.grave_act) {
		im.setTranslation(vector3df(7.3f, 1.7f, dField.grave[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.remove_act) {
		im.setTranslation(vector3df(7.3f, 0.4f, dField.remove[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.extra_act) {
		im.setTranslation(vector3df(0.6f, 3.0f, dField.extra[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.chains.size() > 1) {
		for(int i = 0; i < dField.chains.size(); ++i) {
			if(dField.chains[i].solved)
				break;
			matManager.mTRTexture.setTexture(0, imageManager.tChain);
			matManager.mTRTexture.AmbientColor = 0xffffff00;
			ic.setRotationRadians(act_rot);
			ic.setTranslation(dField.chains[i].chain_pos);
			driver->setMaterial(matManager.mTRTexture);
			driver->setTransform(irr::video::ETS_WORLD, ic);
			driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
			it.setScale(0.6f);
			it.setTranslation(dField.chains[i].chain_pos);
			matManager.mTRTexture.setTexture(0, imageManager.tNumber);
			matManager.vChainNum[0].TCoords = vector2df(0.19375f * (i % 5), 0.2421875f * (i / 5));
			matManager.vChainNum[1].TCoords = vector2df(0.19375f * (i % 5 + 1), 0.2421875f * (i / 5));
			matManager.vChainNum[2].TCoords = vector2df(0.19375f * (i % 5), 0.2421875f * (i / 5 + 1));
			matManager.vChainNum[3].TCoords = vector2df(0.19375f * (i % 5 + 1), 0.2421875f * (i / 5 + 1));
			driver->setMaterial(matManager.mTRTexture);
			driver->setTransform(irr::video::ETS_WORLD, it);
			driver->drawVertexPrimitiveList(matManager.vChainNum, 4, matManager.iRectangle, 2);
		}
	}
	//lp bar
	if((dInfo.turn % 2 && dInfo.isFirst) || (!(dInfo.turn % 2) && !dInfo.isFirst)) {
		driver->draw2DRectangle(0xa0000000, recti(327, 8, 630, 51));
		driver->draw2DRectangleOutline(recti(327, 8, 630, 51), 0xffff8080);
	} else {
		driver->draw2DRectangle(0xa0000000, recti(689, 8, 991, 51));
		driver->draw2DRectangleOutline(recti(689, 8, 991, 51), 0xffff8080);
	}
	driver->draw2DImage(imageManager.tLPFrame, recti(330, 10, 629, 30), recti(0, 0, 200, 20), 0, 0, true);
	driver->draw2DImage(imageManager.tLPFrame, recti(691, 10, 990, 30), recti(0, 0, 200, 20), 0, 0, true);
	if(dInfo.lp[0] >= 8000)
		driver->draw2DImage(imageManager.tLPBar, recti(335, 12, 625, 28), recti(0, 0, 16, 16), 0, 0, true);
	else driver->draw2DImage(imageManager.tLPBar, recti(335, 12, 335 + 290 * dInfo.lp[0] / 8000, 28), recti(0, 0, 16, 16), 0, 0, true);
	if(dInfo.lp[1] >= 8000)
		driver->draw2DImage(imageManager.tLPBar, recti(696, 12, 986, 28), recti(0, 0, 16, 16), 0, 0, true);
	else driver->draw2DImage(imageManager.tLPBar, recti(986 - 290 * dInfo.lp[1] / 8000, 12, 986 , 28), recti(0, 0, 16, 16), 0, 0, true);
	if(lpframe) {
		dInfo.lp[lpplayer] -= lpd;
		myswprintf(dInfo.strLP[lpplayer], L"%d", dInfo.lp[lpplayer]);
		lpccolor -= 0x19000000;
		lpframe--;
	}
	if(lpcstring) {
		if(lpplayer == 0) {
			lpcFont->draw(lpcstring, recti(400, 470, 920, 520), lpccolor | 0x00ffffff, true, false, 0);
			lpcFont->draw(lpcstring, recti(400, 472, 922, 520), lpccolor, true, false, 0);
		} else {
			lpcFont->draw(lpcstring, recti(400, 160, 920, 210), lpccolor | 0x00ffffff, true, false, 0);
			lpcFont->draw(lpcstring, recti(400, 162, 922, 210), lpccolor, true, false, 0);
		}
	}
	if(!dInfo.isReplay && dInfo.player_type < 7 && dInfo.time_limit) {
		driver->draw2DRectangle(recti(525, 34, 525 + dInfo.time_left[0] * 100 / dInfo.time_limit, 44), 0xa0e0e0e0, 0xa0e0e0e0, 0xa0c0c0c0, 0xa0c0c0c0);
		driver->draw2DRectangleOutline(recti(525, 34, 625, 44), 0xffffffff);
		driver->draw2DRectangle(recti(795 - dInfo.time_left[1] * 100 / dInfo.time_limit, 34, 795, 44), 0xa0e0e0e0, 0xa0e0e0e0, 0xa0c0c0c0, 0xa0c0c0c0);
		driver->draw2DRectangleOutline(recti(695, 34, 795, 44), 0xffffffff);
	}
	numFont->draw(dInfo.strLP[0], recti(330, 11, 629, 30), 0xff000000, true, false, 0);
	numFont->draw(dInfo.strLP[0], recti(330, 12, 631, 30), 0xffffff00, true, false, 0);
	numFont->draw(dInfo.strLP[1], recti(691, 11, 990, 30), 0xff000000, true, false, 0);
	numFont->draw(dInfo.strLP[1], recti(691, 12, 992, 30), 0xffffff00, true, false, 0);

	if(!dInfo.isTag || !dInfo.tag_player[0])
		textFont->draw(dInfo.hostname, recti(335, 31, 629, 50), 0xffffffff, false, false, 0);
	else
		textFont->draw(dInfo.hostname_tag, recti(335, 31, 629, 50), 0xffffffff, false, false, 0);
	if(!dInfo.isTag || !dInfo.tag_player[1]) {
		auto cld = textFont->getDimension(dInfo.clientname);
		textFont->draw(dInfo.clientname, recti(986 - cld.Width, 31, 986, 50), 0xffffffff, false, false, 0);
	} else {
		auto cld = textFont->getDimension(dInfo.clientname_tag);
		textFont->draw(dInfo.clientname_tag, recti(986 - cld.Width, 31, 986, 50), 0xffffffff, false, false, 0);
	}
	driver->draw2DRectangle(recti(632, 10, 688, 30), 0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
	driver->draw2DRectangle(recti(632, 30, 688, 50), 0xffffffff, 0xffffffff, 0x00000000, 0x00000000);
	lpcFont->draw(dataManager.GetNumString(dInfo.turn), recti(635, 5, 685, 40), 0x80000000, true, false, 0);
	lpcFont->draw(dataManager.GetNumString(dInfo.turn), recti(635, 5, 687, 40), 0x8000ffff, true, false, 0);
	for(int i = 0; i < 5; ++i) {
		ClientCard* pcard = dField.mzone[0][i];
		if(pcard && pcard->code != 0) {
			int m = 483 + i * 90;
			adFont->draw(L"/", recti(m - 4, 396, m + 4, 416), 0xff000000, true, false, 0);
			adFont->draw(L"/", recti(m - 3, 397, m + 5, 417), 0xffffffff, true, false, 0);
			int w = adFont->getDimension(pcard->atkstring).Width;
			adFont->draw(pcard->atkstring, recti(m - 5 - w, 396, m - 5, 416), 0xff000000, false, false, 0);
			adFont->draw(pcard->atkstring, recti(m - 4 - w, 397, m - 4, 417),
			             pcard->attack > pcard->base_attack ? 0xffffff00 : pcard->attack < pcard->base_attack ? 0xffff2090 : 0xffffffff , false, false, 0);
			w = adFont->getDimension(pcard->defstring).Width;
			adFont->draw(pcard->defstring, recti(m + 4, 396, m + 4 + w, 416), 0xff000000, false, false, 0);
			adFont->draw(pcard->defstring, recti(m + 5, 397, m + 5 + w, 417),
			             pcard->defence > pcard->base_defence ? 0xffffff00 : pcard->defence < pcard->base_defence ? 0xffff2090 : 0xffffffff , false, false, 0);
			adFont->draw(pcard->lvstring, recti(463 + i * 85, 346, 495 + i * 85, 366), 0xff000000, false, false, 0);
			adFont->draw(pcard->lvstring, recti(464 + i * 85, 347, 496 + i * 85, 367),
			             (pcard->type & TYPE_XYZ) ? 0xffff80ff : (pcard->type & TYPE_TUNER) ? 0xffffff00 : 0xffffffff, false, false, 0);
		}
	}
	for(int i = 0; i < 5; ++i) {
		ClientCard* pcard = dField.mzone[1][i];
		if(pcard && (pcard->position & POS_FACEUP)) {
			int m = 810 - i * 73;
			adFont->draw(L"/", recti(m - 4, 245, m + 4, 265), 0xff000000, true, false, 0);
			adFont->draw(L"/", recti(m - 3, 246, m + 5, 266), 0xffffffff, true, false, 0);
			int w = adFont->getDimension(pcard->atkstring).Width;
			adFont->draw(pcard->atkstring, recti(m - 5 - w, 245, m - 5, 265), 0xff000000, false, false, 0);
			adFont->draw(pcard->atkstring, recti(m - 4 - w, 246, m - 4, 266),
			             pcard->attack > pcard->base_attack ? 0xffffff00 : pcard->attack < pcard->base_attack ? 0xffff2090 : 0xffffffff , false, false, 0);
			w = adFont->getDimension(pcard->defstring).Width;
			adFont->draw(pcard->defstring, recti(m + 4, 245, m + 4 + w, 265), 0xff000000, false, false, 0);
			adFont->draw(pcard->defstring, recti(m + 5, 246, m + 5 + w, 266),
			             pcard->defence > pcard->base_defence ? 0xffffff00 : pcard->defence < pcard->base_defence ? 0xffff2090 : 0xffffffff , false, false, 0);
			adFont->draw(pcard->lvstring, recti(789 - i * 76, 282, 820 - i * 76, 302), 0xff000000, false, false, 0);
			adFont->draw(pcard->lvstring, recti(789 - i * 76, 283, 820 - i * 76, 303),
			             (pcard->type & TYPE_XYZ) ? 0xffff80ff : (pcard->type & TYPE_TUNER) ? 0xffffff00 : 0xffffffff, false, false, 0);
		}
	}
	if(dField.extra[0].size()) {
		numFont->draw(dataManager.GetNumString(dField.extra[0].size()), recti(305, 532, 381, 552), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra[0].size()), recti(305, 533, 383, 553), 0xffffff00, true, false, 0);
	}
	if(dField.deck[0].size()) {
		numFont->draw(dataManager.GetNumString(dField.deck[0].size()), recti(942, 532, 1021, 552), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.deck[0].size()), recti(943, 533, 1023, 553), 0xffffff00, true, false, 0);
	}
	if(dField.grave[0].size()) {
		numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(917, 436, 984, 456), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(917, 437, 986, 457), 0xffffff00, true, false, 0);
	}
	if(dField.remove[0].size()) {
		numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(895, 360, 957, 380), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(895, 361, 959, 381), 0xffffff00, true, false, 0);
	}
	if(dField.extra[1].size()) {
		numFont->draw(dataManager.GetNumString(dField.extra[1].size()), recti(848, 212, 908, 232), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra[1].size()), recti(848, 213, 910, 233), 0xffffff00, true, false, 0);
	}
	if(dField.deck[1].size()) {
		numFont->draw(dataManager.GetNumString(dField.deck[1].size()), recti(421, 212, 481, 232), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.deck[1].size()), recti(421, 213, 483, 233), 0xffffff00, true, false, 0);
	}
	if(dField.grave[1].size()) {
		numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(408, 261, 462, 281), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(408, 262, 464, 282), 0xffffff00, true, false, 0);
	}
	if(dField.remove[1].size()) {
		numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(386, 320, 443, 340), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(386, 321, 445, 341), 0xffffff00, true, false, 0);
	}
}
void Game::DrawGUI() {
	if(imageLoading.size()) {
		std::map<irr::gui::CGUIImageButton*, int>::iterator mit;
		for(mit = imageLoading.begin(); mit != imageLoading.end(); ++mit)
			mit->first->setImage(imageManager.GetTexture(mit->second));
		imageLoading.clear();
	}
	for(auto fit = fadingList.begin(); fit != fadingList.end();) {
		auto fthis = fit++;
		FadingUnit& fu = *fthis;
		if(fu.fadingFrame) {
			fu.guiFading->setVisible(true);
			if(fu.isFadein) {
				if(fu.fadingFrame > 5) {
					fu.fadingUL.X -= fu.fadingDiff.X;
					fu.fadingLR.X += fu.fadingDiff.X;
					fu.fadingFrame--;
					fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				} else {
					fu.fadingUL.Y -= fu.fadingDiff.Y;
					fu.fadingLR.Y += fu.fadingDiff.Y;
					fu.fadingFrame--;
					if(!fu.fadingFrame) {
						fu.guiFading->setRelativePosition(fu.fadingSize);
						if(fu.guiFading == wPosSelect) {
							btnPSAU->setDrawImage(true);
							btnPSAD->setDrawImage(true);
							btnPSDU->setDrawImage(true);
							btnPSDD->setDrawImage(true);
						}
						if(fu.guiFading == wCardSelect) {
							for(int i = 0; i < 5; ++i)
								btnCardSelect[i]->setDrawImage(true);
						}
						env->setFocus(fu.guiFading);
					} else
						fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				}
			} else {
				if(fu.fadingFrame > 5) {
					fu.fadingUL.Y += fu.fadingDiff.Y;
					fu.fadingLR.Y -= fu.fadingDiff.Y;
					fu.fadingFrame--;
					fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				} else {
					fu.fadingUL.X += fu.fadingDiff.X;
					fu.fadingLR.X -= fu.fadingDiff.X;
					fu.fadingFrame--;
					if(!fu.fadingFrame) {
						fu.guiFading->setVisible(false);
						fu.guiFading->setRelativePosition(fu.fadingSize);
						if(fu.guiFading == wPosSelect) {
							btnPSAU->setDrawImage(true);
							btnPSAD->setDrawImage(true);
							btnPSDU->setDrawImage(true);
							btnPSDD->setDrawImage(true);
						}
						if(fu.guiFading == wCardSelect) {
							for(int i = 0; i < 5; ++i)
								btnCardSelect[i]->setDrawImage(true);
						}
					} else
						fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				}
				if(fu.signalAction && !fu.fadingFrame) {
					DuelClient::SendResponse();
					fu.signalAction = false;
				}
			}
		} else if(fu.autoFadeoutFrame) {
			fu.autoFadeoutFrame--;
			if(!fu.autoFadeoutFrame)
				HideElement(fu.guiFading);
		} else
			fadingList.erase(fthis);
	}
	env->drawAll();
}
void Game::DrawSpec() {
	if(showcard) {
		switch(showcard) {
		case 1: {
			driver->draw2DImage(imageManager.GetTexture(showcardcode), position2di(574, 150));
			driver->draw2DImage(imageManager.tMask, recti(574, 150, 574 + (showcarddif > 177 ? 177 : showcarddif), 404),
			                    recti(254 - showcarddif, 0, 254 - (showcarddif > 177 ? showcarddif - 177 : 0), 254), 0, 0, true);
			showcarddif += 15;
			if(showcarddif >= 254) {
				showcard = 2;
				showcarddif = 0;
			}
			break;
		}
		case 2: {
			driver->draw2DImage(imageManager.GetTexture(showcardcode), position2di(574, 150));
			driver->draw2DImage(imageManager.tMask, recti(574 + showcarddif, 150, 761, 404), recti(0, 0, 177 - showcarddif, 254), 0, 0, true);
			showcarddif += 15;
			if(showcarddif >= 177) {
				showcard = 0;
			}
			break;
		}
		case 3: {
			driver->draw2DImage(imageManager.GetTexture(showcardcode), position2di(574, 150));
			driver->draw2DImage(imageManager.tNegated, recti(536 + showcarddif, 141 + showcarddif, 793 - showcarddif, 397 - showcarddif), recti(0, 0, 128, 128), 0, 0, true);
			if(showcarddif < 64)
				showcarddif += 4;
			break;
		}
		case 4: {
			matManager.c2d[0] = (showcarddif << 24) | 0xffffff;
			matManager.c2d[1] = (showcarddif << 24) | 0xffffff;
			matManager.c2d[2] = (showcarddif << 24) | 0xffffff;
			matManager.c2d[3] = (showcarddif << 24) | 0xffffff;
			driver->draw2DImage(imageManager.GetTexture(showcardcode), recti(574, 154, 751, 404),
			                    recti(0, 0, 177, 254), 0, matManager.c2d, true);
			if(showcarddif < 255)
				showcarddif += 17;
			break;
		}
		case 5: {
			matManager.c2d[0] = (showcarddif << 25) | 0xffffff;
			matManager.c2d[1] = (showcarddif << 25) | 0xffffff;
			matManager.c2d[2] = (showcarddif << 25) | 0xffffff;
			matManager.c2d[3] = (showcarddif << 25) | 0xffffff;
			driver->draw2DImage(imageManager.GetTexture(showcardcode), recti(662 - showcarddif * 0.69685f, 277 - showcarddif, 662 + showcarddif * 0.69685f, 277 + showcarddif),
			                    recti(0, 0, 177, 254), 0, matManager.c2d, true);
			if(showcarddif < 127)
				showcarddif += 9;
			break;
		}
		case 6: {
			driver->draw2DImage(imageManager.GetTexture(showcardcode), position2di(574, 150));
			driver->draw2DImage(imageManager.tNumber, recti(536 + showcarddif, 141 + showcarddif, 793 - showcarddif, 397 - showcarddif),
			                    recti((showcardp % 5) * 64, (showcardp / 5) * 64, (showcardp % 5 + 1) * 64, (showcardp / 5 + 1) * 64), 0, 0, true);
			if(showcarddif < 64)
				showcarddif += 4;
			break;
		}
		case 7: {
			core::position2d<s32> corner[4];
			float y = sin(showcarddif * 3.1415926f / 180.0f) * 254;
			corner[0] = core::position2d<s32>(574 - (254 - y) * 0.3f, 404 - y);
			corner[1] = core::position2d<s32>(751 + (254 - y) * 0.3f, 404 - y);
			corner[2] = core::position2d<s32>(574, 404);
			corner[3] = core::position2d<s32>(751, 404);
			irr::gui::Draw2DImageQuad(driver, imageManager.GetTexture(showcardcode), rect<s32>(0, 0, 177, 254), corner);
			showcardp++;
			showcarddif += 9;
			if(showcarddif >= 90)
				showcarddif = 90;
			if(showcardp == 60) {
				showcardp = 0;
				showcarddif = 0;
			}
			break;
		}
		case 100: {
			if(showcardp < 60) {
				driver->draw2DImage(imageManager.tHand[(showcardcode >> 16) & 0x3], position2di(615, showcarddif));
				driver->draw2DImage(imageManager.tHand[showcardcode & 0x3], position2di(615, 540 - showcarddif));
				float dy = -0.333333f * showcardp + 10;
				showcardp++;
				if(showcardp < 30)
					showcarddif += (int)dy;
			} else
				showcard = 0;
			break;
		}
		case 101: {
			const wchar_t* lstr = L"";
			switch(showcardcode) {
			case 1:
				lstr = L"You Win!";
				break;
			case 2:
				lstr = L"You Lose!";
				break;
			case 3:
				lstr = L"Draw Game";
				break;
			case 4:
				lstr = L"Draw Phase";
				break;
			case 5:
				lstr = L"Standby Phase";
				break;
			case 6:
				lstr = L"Main Phase 1";
				break;
			case 7:
				lstr = L"Battle Phase";
				break;
			case 8:
				lstr = L"Main Phase 2";
				break;
			case 9:
				lstr = L"End Phase";
				break;
			case 10:
				lstr = L"Next Players Turn";
				break;
			case 11:
				lstr = L"Duel Start";
				break;
			case 12:
				lstr = L"Duel1 Start";
				break;
			case 13:
				lstr = L"Duel2 Start";
				break;
			case 14:
				lstr = L"Duel3 Start";
				break;
			}
			auto pos = lpcFont->getDimension(lstr);
			if(showcardp < 10) {
				int alpha = (showcardp * 25) << 24;
				lpcFont->draw(lstr, recti(671 - pos.Width / 2 - (9 - showcardp) * 40, 271, 970, 350), alpha);
				lpcFont->draw(lstr, recti(670 - pos.Width / 2 - (9 - showcardp) * 40, 270, 970, 350), alpha | 0xffffff);
			} else if(showcardp < showcarddif) {
				lpcFont->draw(lstr, recti(671 - pos.Width / 2, 271, 970, 350), 0xff000000);
				lpcFont->draw(lstr, recti(670 - pos.Width / 2, 270, 970, 350), 0xffffffff);
				if(dInfo.vic_string && (showcardcode == 1 || showcardcode == 2)) {
					driver->draw2DRectangle(0xa0000000, recti(540, 320, 800, 340));
					guiFont->draw(dInfo.vic_string, recti(502, 321, 840, 340), 0xff000000, true, true);
					guiFont->draw(dInfo.vic_string, recti(500, 320, 840, 340), 0xffffffff, true, true);
				}
			} else if(showcardp < showcarddif + 10) {
				int alpha = ((showcarddif + 10 - showcardp) * 25) << 24;
				lpcFont->draw(lstr, recti(671 - pos.Width / 2 + (showcardp - showcarddif) * 40, 271, 970, 350), alpha);
				lpcFont->draw(lstr, recti(670 - pos.Width / 2 + (showcardp - showcarddif) * 40, 270, 970, 350), alpha | 0xffffff);
			}
			showcardp++;
			break;
		}
		}
	}
	if(is_attacking) {
		irr::core::matrix4 matk;
		matk.setTranslation(atk_t);
		matk.setRotationRadians(atk_r);
		driver->setTransform(irr::video::ETS_WORLD, matk);
		driver->setMaterial(matManager.mATK);
		driver->drawVertexPrimitiveList(&matManager.vArrow[attack_sv], 40, matManager.iArrow, 10, EVT_STANDARD, EPT_TRIANGLE_STRIP);
		attack_sv += 4;
		if (attack_sv > 28)
			attack_sv = 0;
	}
	for(int i = 0; i < 5; ++i) {
		static unsigned int chatColor[10] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff8080ff, 0xffff4040};
		if(chatTiming[i]) {
			chatTiming[i]--;
			int w = textFont->getDimension(chatMsg[i].c_str()).Width;
			driver->draw2DRectangle(recti(305, 596 - 20 * i, 307 + w, 616 - 20 * i), 0xa0000000, 0xa0000000, 0xa0000000, 0xa0000000);
			textFont->draw(chatMsg[i].c_str(), rect<s32>(305, 595 - 20 * i, 1020, 615 - 20 * i), 0xff000000, false, false);
			textFont->draw(chatMsg[i].c_str(), rect<s32>(306, 596 - 20 * i, 1021, 616 - 20 * i), chatColor[chatType[i]], false, false);
		}
	}
}
void Game::ShowElement(irr::gui::IGUIElement * win, int autoframe) {
	FadingUnit fu;
	fu.fadingSize = win->getRelativePosition();
	for(auto fit = fadingList.begin(); fit != fadingList.end(); ++fit)
		if(win == fit->guiFading)
			fu.fadingSize = fit->fadingSize;
	irr::core::position2di center = fu.fadingSize.getCenter();
	fu.fadingDiff.X = fu.fadingSize.getWidth() / 10;
	fu.fadingDiff.Y = (fu.fadingSize.getHeight() - 4) / 10;
	fu.fadingUL = center;
	fu.fadingLR = center;
	fu.fadingUL.Y -= 2;
	fu.fadingLR.Y += 2;
	fu.guiFading = win;
	fu.isFadein = true;
	fu.fadingFrame = 10;
	fu.autoFadeoutFrame = autoframe;
	if(win == wPosSelect) {
		btnPSAU->setDrawImage(false);
		btnPSAD->setDrawImage(false);
		btnPSDU->setDrawImage(false);
		btnPSDD->setDrawImage(false);
	}
	if(win == wCardSelect) {
		for(int i = 0; i < 5; ++i)
			btnCardSelect[i]->setDrawImage(false);
	}
	win->setRelativePosition(irr::core::recti(center.X, center.Y, 0, 0));
	fadingList.push_back(fu);
}
void Game::HideElement(irr::gui::IGUIElement * win, bool set_action) {
	FadingUnit fu;
	fu.fadingSize = win->getRelativePosition();
	for(auto fit = fadingList.begin(); fit != fadingList.end(); ++fit)
		if(win == fit->guiFading)
			fu.fadingSize = fit->fadingSize;
	fu.fadingDiff.X = fu.fadingSize.getWidth() / 10;
	fu.fadingDiff.Y = (fu.fadingSize.getHeight() - 4) / 10;
	fu.fadingUL = fu.fadingSize.UpperLeftCorner;
	fu.fadingLR = fu.fadingSize.LowerRightCorner;
	fu.guiFading = win;
	fu.isFadein = false;
	fu.fadingFrame = 10;
	fu.autoFadeoutFrame = 0;
	fu.signalAction = set_action;
	if(win == wPosSelect) {
		btnPSAU->setDrawImage(false);
		btnPSAD->setDrawImage(false);
		btnPSDU->setDrawImage(false);
		btnPSDD->setDrawImage(false);
	}
	if(win == wCardSelect) {
		for(int i = 0; i < 5; ++i)
			btnCardSelect[i]->setDrawImage(false);
	}
	fadingList.push_back(fu);
}
void Game::PopupElement(irr::gui::IGUIElement * element, int hideframe) {
	element->getParent()->bringToFront(element);
	dField.panel = element;
	env->setFocus(element);
	if(!hideframe)
		ShowElement(element);
	else ShowElement(element, hideframe);
}
void Game::WaitFrameSignal(int frame) {
	frameSignal.Reset();
	signalFrame = frame;
	frameSignal.Wait();
}
void Game::DrawThumb(code_pointer cp, position2di pos, std::unordered_map<int, int>* lflist) {
	const int width = 44; //standard pic size, maybe it should be defined in game.h
	const int height = 64;
	int code = cp->first;
	int lcode = cp->second.alias;
	if(lcode == 0)
		lcode = code;
	irr::video::ITexture* img = imageManager.GetTextureThumb(code);
	if(img == NULL)
		return; //NULL->getSize() will cause a crash
	dimension2d<u32> size = img->getOriginalSize();
	driver->draw2DImage(img, rect<s32>(pos.X, pos.Y, pos.X + width, pos.Y + height), rect<s32>(0, 0, size.Width, size.Height));

	if(lflist->count(lcode)) {
		switch((*lflist)[lcode]) {
		case 0:
			driver->draw2DImage(imageManager.tLim, recti(pos.X, pos.Y, pos.X + 20, pos.Y + 20), recti(0, 0, 64, 64), 0, 0, true);
			break;
		case 1:
			driver->draw2DImage(imageManager.tLim, recti(pos.X, pos.Y, pos.X + 20, pos.Y + 20), recti(64, 0, 128, 64), 0, 0, true);
			break;
		case 2:
			driver->draw2DImage(imageManager.tLim, recti(pos.X, pos.Y, pos.X + 20, pos.Y + 20), recti(0, 64, 64, 128), 0, 0, true);
			break;
		}
	}
}
void Game::DrawDeckBd() {
	wchar_t textBuffer[32];
	//main deck
	driver->draw2DRectangle(recti(310, 137, 410, 157), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 136, 410, 157));
	textFont->draw(dataManager.GetSysString(1330), recti(314, 136, 409, 156), 0xff000000, false, true);
	textFont->draw(dataManager.GetSysString(1330), recti(315, 137, 410, 157), 0xffffffff, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.main.size()], recti(379, 137, 439, 157), 0xff000000, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.main.size()], recti(380, 138, 440, 158), 0xffffffff, false, true);
	driver->draw2DRectangle(recti(310, 160, 797, 436), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 159, 797, 436));
	int lx;
	float dx;
	if(deckManager.current_deck.main.size() <= 40) {
		dx = 436.0f / 9;
		lx = 10;
	} else {
		lx = (deckManager.current_deck.main.size() - 41) / 4 + 11;
		dx = 436.0f / (lx - 1);
	}
	for(int i = 0; i < deckManager.current_deck.main.size(); ++i) {
		DrawThumb(deckManager.current_deck.main[i], position2di(314 + (i % lx) * dx, 164 + (i / lx) * 68), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 1 && deckBuilder.hovered_seq == i)
			driver->draw2DRectangleOutline(recti(313 + (i % lx) * dx, 163 + (i / lx) * 68, 359 + (i % lx) * dx, 228 + (i / lx) * 68));
	}
	//extra deck
	driver->draw2DRectangle(recti(310, 440, 410, 460), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 439, 410, 460));
	textFont->draw(dataManager.GetSysString(1331), recti(314, 439, 409, 459), 0xff000000, false, true);
	textFont->draw(dataManager.GetSysString(1331), recti(315, 440, 410, 460), 0xffffffff, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.extra.size()], recti(379, 440, 439, 460), 0xff000000, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.extra.size()], recti(380, 441, 440, 461), 0xffffffff, false, true);
	driver->draw2DRectangle(recti(310, 463, 797, 533), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 462, 797, 533));
	if(deckManager.current_deck.extra.size() <= 10)
		dx = 436.0f / 9;
	else dx = 436.0f / (deckManager.current_deck.extra.size() - 1);
	for(int i = 0; i < deckManager.current_deck.extra.size(); ++i) {
		DrawThumb(deckManager.current_deck.extra[i], position2di(314 + i * dx, 466), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 2 && deckBuilder.hovered_seq == i)
			driver->draw2DRectangleOutline(recti(313 + i * dx, 465, 359 + i * dx, 531));
	}
	//side deck
	driver->draw2DRectangle(recti(310, 537, 410, 557), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 536, 410, 557));
	textFont->draw(dataManager.GetSysString(1332), recti(314, 536, 409, 556), 0xff000000, false, true);
	textFont->draw(dataManager.GetSysString(1332), recti(315, 537, 410, 557), 0xffffffff, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.side.size()], recti(379, 537, 439, 557), 0xff000000, false, true);
	numFont->draw(dataManager.numStrings[deckManager.current_deck.side.size()], recti(380, 538, 440, 558), 0xffffffff, false, true);
	driver->draw2DRectangle(recti(310, 560, 797, 630), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(309, 559, 797, 630));
	if(deckManager.current_deck.side.size() <= 10)
		dx = 436.0f / 9;
	else dx = 436.0f / (deckManager.current_deck.side.size() - 1);
	for(int i = 0; i < deckManager.current_deck.side.size(); ++i) {
		DrawThumb(deckManager.current_deck.side[i], position2di(314 + i * dx, 564), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 3 && deckBuilder.hovered_seq == i)
			driver->draw2DRectangleOutline(recti(313 + i * dx, 563, 359 + i * dx, 629));
	}
	driver->draw2DRectangle(recti(805, 137, 915, 157), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(804, 136, 915, 157));
	textFont->draw(dataManager.GetSysString(1333), recti(809, 136, 914, 156), 0xff000000, false, true);
	textFont->draw(dataManager.GetSysString(1333), recti(810, 137, 915, 157), 0xffffffff, false, true);
	numFont->draw(deckBuilder.result_string, recti(874, 136, 934, 156), 0xff000000, false, true);
	numFont->draw(deckBuilder.result_string, recti(875, 137, 935, 157), 0xffffffff, false, true);
	driver->draw2DRectangle(recti(805, 160, 1020, 630), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(804, 159, 1020, 630));
	for(int i = 0; i < 7 && i + mainGame->scrFilter->getPos() < deckBuilder.results.size(); ++i) {
		code_pointer ptr = deckBuilder.results[i + mainGame->scrFilter->getPos()];
		if(deckBuilder.hovered_pos == 4 && deckBuilder.hovered_seq == i)
			driver->draw2DRectangle(0x80000000, recti(806, 164 + i * 66, 1019, 230 + i * 66));
		DrawThumb(ptr, position2di(810, 165 + i * 66), deckBuilder.filterList);
		if(ptr->second.type & TYPE_MONSTER) {
			myswprintf(textBuffer, L"%ls", dataManager.GetName(ptr->first));
			textFont->draw(textBuffer, recti(859, 164 + i * 66, 955, 185 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 165 + i * 66, 955, 185 + i * 66), 0xffffffff, false, false);
			myswprintf(textBuffer, L"%ls/%ls \x2605%d", dataManager.FormatAttribute(ptr->second.attribute), dataManager.FormatRace(ptr->second.race), ptr->second.level);
			textFont->draw(textBuffer, recti(859, 186 + i * 66, 955, 207 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 187 + i * 66, 955, 207 + i * 66), 0xffffffff, false, false);
			if(ptr->second.attack < 0 && ptr->second.defence < 0)
				myswprintf(textBuffer, L"?/?");
			else if(ptr->second.attack < 0)
				myswprintf(textBuffer, L"?/%d", ptr->second.defence);
			else if(ptr->second.defence < 0)
				myswprintf(textBuffer, L"%d/?", ptr->second.attack);
			else myswprintf(textBuffer, L"%d/%d", ptr->second.attack, ptr->second.defence);
			if((ptr->second.ot & 0x3) == 1)
				wcscat(textBuffer, L" [OCG]");
			else if((ptr->second.ot & 0x3) == 2)
				wcscat(textBuffer, L" [TCG]");
			textFont->draw(textBuffer, recti(859, 208 + i * 66, 955, 229 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 209 + i * 66, 955, 229 + i * 66), 0xffffffff, false, false);
		} else {
			myswprintf(textBuffer, L"%ls", dataManager.GetName(ptr->first));
			textFont->draw(textBuffer, recti(859, 164 + i * 66, 955, 185 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 165 + i * 66, 955, 185 + i * 66), 0xffffffff, false, false);
			const wchar_t* ptype = dataManager.FormatType(ptr->second.type);
			textFont->draw(ptype, recti(859, 186 + i * 66, 955, 207 + i * 66), 0xff000000, false, false);
			textFont->draw(ptype, recti(860, 187 + i * 66, 955, 207 + i * 66), 0xffffffff, false, false);
			textBuffer[0] = 0;
			if((ptr->second.ot & 0x3) == 1)
				wcscat(textBuffer, L"[OCG]");
			else if((ptr->second.ot & 0x3) == 2)
				wcscat(textBuffer, L"[TCG]");
			textFont->draw(textBuffer, recti(859, 208 + i * 66, 955, 229 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 209 + i * 66, 955, 229 + i * 66), 0xffffffff, false, false);
		}
	}
	if(deckBuilder.is_draging) {
		DrawThumb(deckBuilder.draging_pointer, position2di(deckBuilder.dragx - 22, deckBuilder.dragy - 32), deckBuilder.filterList);
	}
}
}
