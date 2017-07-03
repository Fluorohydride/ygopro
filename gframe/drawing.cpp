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
		glEnable(GL_TEXTURE_2D);
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
//	matrix4 im = irr::core::IdentityMatrix;
//	im.setTranslation(vector3df(0, 0, -0.01f));
//	driver->setTransform(irr::video::ETS_WORLD, im);
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
	//draw field spell card
	driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
	bool drawField = false;
	int rule = (dInfo.duel_rule >= 4) ? 1 : 0;
	if(mainGame->gameConf.draw_field_spell) {
		int fieldcode1 = -1;
		int fieldcode2 = -1;
		if(mainGame->dField.szone[0][5] && mainGame->dField.szone[0][5]->position & POS_FACEUP)
			fieldcode1 = mainGame->dField.szone[0][5]->code;
		if(mainGame->dField.szone[1][5] && mainGame->dField.szone[1][5]->position & POS_FACEUP)
			fieldcode2 = mainGame->dField.szone[1][5]->code;
		int fieldcode = (fieldcode1 > 0) ? fieldcode1 : fieldcode2;
		if(fieldcode1 > 0 && fieldcode2 > 0 && fieldcode1 != fieldcode2) {
			ITexture* texture = imageManager.GetTextureField(fieldcode1);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell1, 4, matManager.iRectangle, 2);
			}
			texture = imageManager.GetTextureField(fieldcode2);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell2, 4, matManager.iRectangle, 2);
			}
		} else if(fieldcode > 0) {
			ITexture* texture = imageManager.GetTextureField(fieldcode);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell, 4, matManager.iRectangle, 2);
			}
		}
	}
	matManager.mTexture.setTexture(0, drawField ? imageManager.tFieldTransparent[rule] : imageManager.tField[rule]);
	driver->setMaterial(matManager.mTexture);
	driver->drawVertexPrimitiveList(matManager.vField, 4, matManager.iRectangle, 2);
	driver->setMaterial(matManager.mBackLine);
	//select field
	if(dInfo.curMsg == MSG_SELECT_PLACE || dInfo.curMsg == MSG_SELECT_DISFIELD) {
		float cv[4] = {0.0f, 0.0f, 1.0f, 1.0f};
		unsigned int filter = 0x1;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldMzone[0][i], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x100;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldSzone[0][i][rule], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x10000;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldMzone[1][i], !(dField.selected_field & filter), 2, cv);
		}
		filter = 0x1000000;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldSzone[1][i][rule], !(dField.selected_field & filter), 2, cv);
		}
	}
	//disabled field
	{
		/*float cv[4] = {0.0f, 0.0f, 1.0f, 1.0f};*/
		unsigned int filter = 0x1;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldMzone[0][i][0].Pos, matManager.vFieldMzone[0][i][3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFieldMzone[0][i][1].Pos, matManager.vFieldMzone[0][i][2].Pos, 0xffffffff);
			}
		}
		filter = 0x100;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldSzone[0][i][rule][0].Pos, matManager.vFieldSzone[0][i][rule][3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFieldSzone[0][i][rule][1].Pos, matManager.vFieldSzone[0][i][rule][2].Pos, 0xffffffff);
			}
		}
		filter = 0x10000;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldMzone[1][i][0].Pos, matManager.vFieldMzone[1][i][3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFieldMzone[1][i][1].Pos, matManager.vFieldMzone[1][i][2].Pos, 0xffffffff);
			}
		}
		filter = 0x1000000;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldSzone[1][i][rule][0].Pos, matManager.vFieldSzone[1][i][rule][3].Pos, 0xffffffff);
				driver->draw3DLine(matManager.vFieldSzone[1][i][rule][1].Pos, matManager.vFieldSzone[1][i][rule][2].Pos, 0xffffffff);
			}
		}
	}
	//current sel
	if (dField.hovered_location != 0 && dField.hovered_location != 2 && dField.hovered_location != POSITION_HINT
		&& !(dInfo.duel_rule < 4 && dField.hovered_location == LOCATION_MZONE && dField.hovered_sequence > 4)
		&& !(dInfo.duel_rule >= 4 && dField.hovered_location == LOCATION_SZONE && dField.hovered_sequence > 5)) {
		S3DVertex *vertex = 0;
		if (dField.hovered_location == LOCATION_DECK)
			vertex = matManager.vFieldDeck[dField.hovered_controler];
		else if (dField.hovered_location == LOCATION_MZONE) {
			vertex = matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence];
			ClientCard* pcard = mainGame->dField.mzone[dField.hovered_controler][dField.hovered_sequence];
			if(pcard && pcard->type & TYPE_LINK) {
				DrawLinkedZones(pcard);
			}
		} else if (dField.hovered_location == LOCATION_SZONE)
			vertex = matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence][rule];
		else if (dField.hovered_location == LOCATION_GRAVE)
			vertex = matManager.vFieldGrave[dField.hovered_controler][rule];
		else if (dField.hovered_location == LOCATION_REMOVED)
			vertex = matManager.vFieldRemove[dField.hovered_controler][rule];
		else if (dField.hovered_location == LOCATION_EXTRA)
			vertex = matManager.vFieldExtra[dField.hovered_controler];
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
		driver->drawVertexPrimitiveList(vertex, 4, matManager.iRectangle, 2);
	}
}
void Game::DrawLinkedZones(ClientCard* pcard) {
	int mark = pcard->link_marker;
	ClientCard* pcard2;
	if (dField.hovered_sequence < 5) {
		if (mark & LINK_MARKER_LEFT && dField.hovered_sequence > 0) {
			pcard2 = mainGame->dField.mzone[dField.hovered_controler][dField.hovered_sequence - 1];
			CheckMutual(pcard2, LINK_MARKER_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence - 1], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_RIGHT && dField.hovered_sequence < 4) {
			pcard2 = mainGame->dField.mzone[dField.hovered_controler][dField.hovered_sequence + 1];
			CheckMutual(pcard2, LINK_MARKER_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence + 1], 4, matManager.iRectangle, 2);
		}
		if (dInfo.duel_rule >= 4) {
			if ((mark & LINK_MARKER_TOP_LEFT && dField.hovered_sequence == 2)
				|| (mark & LINK_MARKER_TOP && dField.hovered_sequence == 1)
				|| (mark & LINK_MARKER_TOP_RIGHT && dField.hovered_sequence == 0)) {
				int mark = (dField.hovered_sequence == 2) ? LINK_MARKER_BOTTOM_RIGHT : (dField.hovered_sequence == 1) ? LINK_MARKER_BOTTOM : LINK_MARKER_BOTTOM_LEFT;
				pcard2 = mainGame->dField.mzone[dField.hovered_controler][5];
				if (!pcard2) {
					pcard2 = mainGame->dField.mzone[1 - dField.hovered_controler][6];
					mark = (dField.hovered_sequence == 2) ? LINK_MARKER_TOP_LEFT : (dField.hovered_sequence == 1) ? LINK_MARKER_TOP : LINK_MARKER_TOP_RIGHT;
				}
				CheckMutual(pcard2, mark);
				driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][5], 4, matManager.iRectangle, 2);
			}
			if ((mark & LINK_MARKER_TOP_LEFT && dField.hovered_sequence == 4)
				|| (mark & LINK_MARKER_TOP && dField.hovered_sequence == 3)
				|| (mark & LINK_MARKER_TOP_RIGHT && dField.hovered_sequence == 2)) {
				int mark = (dField.hovered_sequence == 4) ? LINK_MARKER_BOTTOM_RIGHT : (dField.hovered_sequence == 3) ? LINK_MARKER_BOTTOM : LINK_MARKER_BOTTOM_LEFT;
				pcard2 = mainGame->dField.mzone[dField.hovered_controler][6];
				if (!pcard2) {
					pcard2 = mainGame->dField.mzone[1 - dField.hovered_controler][5];
					mark = (dField.hovered_sequence == 4) ? LINK_MARKER_TOP_LEFT : (dField.hovered_sequence == 3) ? LINK_MARKER_TOP : LINK_MARKER_TOP_RIGHT;
				}
				CheckMutual(pcard2, mark);
				driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][6], 4, matManager.iRectangle, 2);
			}
		}
	} else {
		int swap = (dField.hovered_sequence == 5) ? 0 : 2;
		if (mark & LINK_MARKER_BOTTOM_LEFT) {
			pcard2 = mainGame->dField.mzone[dField.hovered_controler][0 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][0 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_BOTTOM) {
			pcard2 = mainGame->dField.mzone[dField.hovered_controler][1 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][1 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_BOTTOM_RIGHT) {
			pcard2 = mainGame->dField.mzone[dField.hovered_controler][2 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][2 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP_LEFT) {
			pcard2 = mainGame->dField.mzone[1 - dField.hovered_controler][4 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][4 - swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP) {
			pcard2 = mainGame->dField.mzone[1 - dField.hovered_controler][3 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][3 - swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP_RIGHT) {
			pcard2 = mainGame->dField.mzone[1 - dField.hovered_controler][2 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][2 - swap], 4, matManager.iRectangle, 2);
		}
	}
}
void Game::CheckMutual(ClientCard* pcard, int mark) {
	matManager.mSelField.AmbientColor = 0xff0261a2;
	driver->setMaterial(matManager.mSelField);
	if (pcard && pcard->type & TYPE_LINK && pcard->link_marker & mark) {
		matManager.mSelField.AmbientColor = 0xff009900;
		driver->setMaterial(matManager.mSelField);
	}
}
void Game::DrawCards() {
	for(int p = 0; p < 2; ++p) {
		for(auto it = dField.mzone[p].begin(); it != dField.mzone[p].end(); ++it)
			if(*it)
				DrawCard(*it);
		for(auto it = dField.szone[p].begin(); it != dField.szone[p].end(); ++it)
			if(*it)
				DrawCard(*it);
		for(auto it = dField.deck[p].begin(); it != dField.deck[p].end(); ++it)
			DrawCard(*it);
		for(auto it = dField.hand[p].begin(); it != dField.hand[p].end(); ++it)
			DrawCard(*it);
		for(auto it = dField.grave[p].begin(); it != dField.grave[p].end(); ++it)
			DrawCard(*it);
		for(auto it = dField.remove[p].begin(); it != dField.remove[p].end(); ++it)
			DrawCard(*it);
		for(auto it = dField.extra[p].begin(); it != dField.extra[p].end(); ++it)
			DrawCard(*it);
	}
	for(auto cit = dField.overlay_cards.begin(); cit != dField.overlay_cards.end(); ++cit)
		DrawCard(*cit);
}
void Game::DrawCard(ClientCard* pcard) {
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
	driver->setTransform(irr::video::ETS_WORLD, pcard->mTransform);
	auto m22 = pcard->mTransform(2, 2);
	if(m22 > -0.99 || pcard->is_moving) {
		matManager.mCard.setTexture(0, imageManager.GetTexture(pcard->code));
		driver->setMaterial(matManager.mCard);
		driver->drawVertexPrimitiveList(matManager.vCardFront, 4, matManager.iRectangle, 2);
	}
	if(m22 < 0.99 || pcard->is_moving) {
		matManager.mCard.setTexture(0, imageManager.tCover[pcard->controler]);
		driver->setMaterial(matManager.mCard);
		driver->drawVertexPrimitiveList(matManager.vCardBack, 4, matManager.iRectangle, 2);
	}
	if(pcard->is_moving)
		return;
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
	irr::core::matrix4 im;
	im.setTranslation(pcard->curPos);
	driver->setTransform(irr::video::ETS_WORLD, im);
	if(pcard->is_showequip) {
		matManager.mTexture.setTexture(0, imageManager.tEquip);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	} else if(pcard->is_showtarget) {
		matManager.mTexture.setTexture(0, imageManager.tTarget);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	} else if(pcard->is_showchaintarget) {
		matManager.mTexture.setTexture(0, imageManager.tChainTarget);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	} else if(pcard->is_disabled && (pcard->location & LOCATION_ONFIELD) && (pcard->position & POS_FACEUP)) {
		matManager.mTexture.setTexture(0, imageManager.tNegated);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vNegate, 4, matManager.iRectangle, 2);
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
	int rule = (dInfo.duel_rule >= 4) ? 1 : 0;
	irr::core::matrix4 im, ic, it;
	act_rot.Z += 0.02f;
	im.setRotationRadians(act_rot);
	matManager.mTexture.setTexture(0, imageManager.tAct);
	driver->setMaterial(matManager.mTexture);
	if(dField.deck_act) {
		im.setTranslation(vector3df((matManager.vFieldDeck[0][0].Pos.X + matManager.vFieldDeck[0][1].Pos.X) / 2,
			(matManager.vFieldDeck[0][0].Pos.Y + matManager.vFieldDeck[0][2].Pos.Y) / 2, dField.deck[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.grave_act) {
		im.setTranslation(vector3df((matManager.vFieldGrave[0][rule][0].Pos.X + matManager.vFieldGrave[0][rule][1].Pos.X) / 2,
			(matManager.vFieldGrave[0][rule][0].Pos.Y + matManager.vFieldGrave[0][rule][2].Pos.Y) / 2, dField.grave[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.remove_act) {
		im.setTranslation(vector3df((matManager.vFieldRemove[0][rule][0].Pos.X + matManager.vFieldRemove[0][rule][1].Pos.X) / 2,
			(matManager.vFieldRemove[0][rule][0].Pos.Y + matManager.vFieldRemove[0][rule][2].Pos.Y) / 2, dField.remove[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.extra_act) {
		im.setTranslation(vector3df((matManager.vFieldExtra[0][0].Pos.X + matManager.vFieldExtra[0][1].Pos.X) / 2,
			(matManager.vFieldExtra[0][0].Pos.Y + matManager.vFieldExtra[0][2].Pos.Y) / 2, dField.extra[0].size() * 0.01f + 0.02f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.pzone_act[0]) {
		int seq = dInfo.duel_rule >= 4 ? 0 : 6;
		im.setTranslation(vector3df((matManager.vFieldSzone[0][seq][rule][0].Pos.X + matManager.vFieldSzone[0][seq][rule][1].Pos.X) / 2,
			(matManager.vFieldSzone[0][seq][rule][0].Pos.Y + matManager.vFieldSzone[0][seq][rule][2].Pos.Y) / 2, 0.03f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.pzone_act[1]) {
		int seq = dInfo.duel_rule >= 4 ? 0 : 6;
		im.setTranslation(vector3df((matManager.vFieldSzone[1][seq][rule][0].Pos.X + matManager.vFieldSzone[1][seq][rule][1].Pos.X) / 2,
			(matManager.vFieldSzone[1][seq][rule][0].Pos.Y + matManager.vFieldSzone[1][seq][rule][2].Pos.Y) / 2, 0.03f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.conti_act) {
		im.setTranslation(vector3df((matManager.vFieldContiAct[0].X + matManager.vFieldContiAct[1].X) / 2,
			(matManager.vFieldContiAct[0].Y + matManager.vFieldContiAct[2].Y) / 2, 0.03f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}
	if(dField.chains.size() > 1) {
		for(size_t i = 0; i < dField.chains.size(); ++i) {
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
	else driver->draw2DImage(imageManager.tLPBar, recti(986 - 290 * dInfo.lp[1] / 8000, 12, 986, 28), recti(0, 0, 16, 16), 0, 0, true);
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
	ClientCard* pcard;
	for(int i = 0; i < 5; ++i) {
		pcard = dField.mzone[0][i];
		if(pcard && pcard->code != 0)
			DrawStatus(pcard, 493 + i * 85, 416, 473 + i * 80, 356);
	}
	pcard = dField.mzone[0][5];
	if(pcard && pcard->code != 0)
		DrawStatus(pcard, 589, 338, 563, 291);
	pcard = dField.mzone[0][6];
	if(pcard && pcard->code != 0)
		DrawStatus(pcard, 743, 338, 712, 291);
	for(int i = 0; i < 5; ++i) {
		pcard = dField.mzone[1][i];
		if(pcard && (pcard->position & POS_FACEUP))
			DrawStatus(pcard, 803 - i * 68, 235, 779 - i * 71, 272);
	}
	pcard = dField.mzone[1][5];
	if(pcard && (pcard->position & POS_FACEUP))
		DrawStatus(pcard, 739, 291, 710, 338);
	pcard = dField.mzone[1][6];
	if(pcard && (pcard->position & POS_FACEUP))
		DrawStatus(pcard, 593, 291, 555, 338);
	if(dInfo.duel_rule < 4) {
		pcard = dField.szone[0][6];
		if(pcard) {
			adFont->draw(pcard->lscstring, recti(426, 394, 438, 414), 0xff000000, true, false, 0);
			adFont->draw(pcard->lscstring, recti(427, 395, 439, 415), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[0][7];
		if(pcard) {
			adFont->draw(pcard->rscstring, recti(880, 394, 912, 414), 0xff000000, true, false, 0);
			adFont->draw(pcard->rscstring, recti(881, 395, 913, 415), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[1][6];
		if(pcard) {
			adFont->draw(pcard->lscstring, recti(839, 245, 871, 265), 0xff000000, true, false, 0);
			adFont->draw(pcard->lscstring, recti(840, 246, 872, 266), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[1][7];
		if(pcard) {
			adFont->draw(pcard->rscstring, recti(463, 245, 495, 265), 0xff000000, true, false, 0);
			adFont->draw(pcard->rscstring, recti(464, 246, 496, 266), 0xffffffff, true, false, 0);
		}
	} else {
		pcard = dField.szone[0][0];
		if(pcard && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget) {
			adFont->draw(pcard->lscstring, recti(454, 430, 466, 450), 0xff000000, true, false, 0);
			adFont->draw(pcard->lscstring, recti(455, 431, 467, 451), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[0][4];
		if(pcard && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget) {
			adFont->draw(pcard->rscstring, recti(850, 430, 882, 450), 0xff000000, true, false, 0);
			adFont->draw(pcard->rscstring, recti(851, 431, 883, 451), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[1][0];
		if(pcard && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget) {
			adFont->draw(pcard->lscstring, recti(806, 222, 838, 242), 0xff000000, true, false, 0);
			adFont->draw(pcard->lscstring, recti(807, 223, 839, 243), 0xffffffff, true, false, 0);
		}
		pcard = dField.szone[1][4];
		if(pcard && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget) {
			adFont->draw(pcard->rscstring, recti(498, 222, 530, 242), 0xff000000, true, false, 0);
			adFont->draw(pcard->rscstring, recti(499, 223, 531, 243), 0xffffffff, true, false, 0);
		}
	}
	if(dField.extra[0].size()) {
		int offset = (dField.extra[0].size() >= 10) ? 0 : mainGame->textFont->getDimension(dataManager.GetNumString(1)).Width;
		numFont->draw(dataManager.GetNumString(dField.extra[0].size()), recti(320 + offset, 562, 371, 552), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra[0].size()), recti(320 + offset, 563, 373, 553), 0xffffff00, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra_p_count[0], true), recti(340, 562, 391, 552), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra_p_count[0], true), recti(340, 563, 393, 553), 0xffffff00, true, false, 0);
	}
	if(dField.deck[0].size()) {
		numFont->draw(dataManager.GetNumString(dField.deck[0].size()), recti(907, 562, 1021, 552), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.deck[0].size()), recti(908, 563, 1023, 553), 0xffffff00, true, false, 0);
	}
	if (rule == 0) {
		if (dField.grave[0].size()) {
			numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(837, 375, 984, 380), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(837, 376, 986, 381), 0xffffff00, true, false, 0);
		}
		if (dField.remove[0].size()) {
			numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(1015, 375, 957, 380), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(1015, 376, 959, 381), 0xffffff00, true, false, 0);
		}
	} else {
		if (dField.grave[0].size()) {
			numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(870, 456, 1002, 461), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.grave[0].size()), recti(870, 457, 1004, 462), 0xffffff00, true, false, 0);
		}
		if (dField.remove[0].size()) {
			numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(837, 375, 984, 380), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.remove[0].size()), recti(837, 376, 986, 381), 0xffffff00, true, false, 0);
		}
	}
	if(dField.extra[1].size()) {
		int offset = (dField.extra[1].size() >= 10) ? 0 : mainGame->textFont->getDimension(dataManager.GetNumString(1)).Width;
		numFont->draw(dataManager.GetNumString(dField.extra[1].size()), recti(808 + offset, 207, 898, 232), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra[1].size()), recti(808 + offset, 208, 900, 233), 0xffffff00, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra_p_count[1], true), recti(828, 207, 918, 232), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.extra_p_count[1], true), recti(828, 208, 920, 233), 0xffffff00, true, false, 0);
	}
	if(dField.deck[1].size()) {
		numFont->draw(dataManager.GetNumString(dField.deck[1].size()), recti(465, 207, 481, 232), 0xff000000, true, false, 0);
		numFont->draw(dataManager.GetNumString(dField.deck[1].size()), recti(465, 208, 483, 233), 0xffffff00, true, false, 0);
	}
	if (rule == 0) {
		if (dField.grave[1].size()) {
			numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(420, 310, 462, 281), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(420, 311, 464, 282), 0xffffff00, true, false, 0);
		}
		if (dField.remove[1].size()) {
			numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(300, 310, 443, 340), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(300, 311, 445, 341), 0xffffff00, true, false, 0);
		}
	} else {
		if (dField.grave[1].size()) {
			numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(455, 249, 462, 299), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.grave[1].size()), recti(455, 250, 464, 300), 0xffffff00, true, false, 0);
		}
		if (dField.remove[1].size()) {
			numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(420, 310, 462, 281), 0xff000000, true, false, 0);
			numFont->draw(dataManager.GetNumString(dField.remove[1].size()), recti(420, 311, 464, 282), 0xffffff00, true, false, 0);
		}
	}
}
void Game::DrawStatus(ClientCard* pcard, int x1, int y1, int x2, int y2) {
	adFont->draw(L"/", recti(x1 - 4, y1, x1 + 4, y1 + 20), 0xff000000, true, false, 0);
	adFont->draw(L"/", recti(x1 - 3, y1 + 1, x1 + 5, y1 + 21), 0xffffffff, true, false, 0);
	int w = adFont->getDimension(pcard->atkstring).Width;
	adFont->draw(pcard->atkstring, recti(x1 - 5 - w, y1, x1 - 5, y1 + 20), 0xff000000, false, false, 0);
	adFont->draw(pcard->atkstring, recti(x1 - 4 - w, y1 + 1, x1 - 4, y1 + 21),
		pcard->attack > pcard->base_attack ? 0xffffff00 : pcard->attack < pcard->base_attack ? 0xffff2090 : 0xffffffff, false, false, 0);
	if(pcard->type & TYPE_LINK) {
		w = adFont->getDimension(pcard->linkstring).Width;
		adFont->draw(pcard->linkstring, recti(x1 + 4, y1, x1 + 4 + w, y1 + 20), 0xff000000, false, false, 0);
		adFont->draw(pcard->linkstring, recti(x1 + 5, y1 + 1, x1 + 5 + w, y1 + 21), 0xff99ffff, false, false, 0);
	} else {
		w = adFont->getDimension(pcard->defstring).Width;
		adFont->draw(pcard->defstring, recti(x1 + 4, y1, x1 + 4 + w, y1 + 20), 0xff000000, false, false, 0);
		adFont->draw(pcard->defstring, recti(x1 + 5, y1 + 1, x1 + 5 + w, y1 + 21),
			pcard->defense > pcard->base_defense ? 0xffffff00 : pcard->defense < pcard->base_defense ? 0xffff2090 : 0xffffffff, false, false, 0);
		adFont->draw(pcard->lvstring, recti(x2, y2, x2 + 2, y2 + 20), 0xff000000, false, false, 0);
		adFont->draw(pcard->lvstring, recti(x2 + 1, y2, x2 + 3, y2 + 21),
			(pcard->type & TYPE_XYZ) ? 0xffff80ff : (pcard->type & TYPE_TUNER) ? 0xffffff00 : 0xffffffff, false, false, 0);
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
						if(fu.guiFading == wCardDisplay) {
							for(int i = 0; i < 5; ++i)
								btnCardDisplay[i]->setDrawImage(true);
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
						if(fu.guiFading == wCardDisplay) {
							for(int i = 0; i < 5; ++i)
								btnCardDisplay[i]->setDrawImage(true);
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
				lpcFont->draw(lstr, recti(651 - pos.Width / 2 - (9 - showcardp) * 40, 291, 950, 370), alpha);
				lpcFont->draw(lstr, recti(650 - pos.Width / 2 - (9 - showcardp) * 40, 290, 950, 370), alpha | 0xffffff);
			} else if(showcardp < showcarddif) {
				recti loc = recti(650 - pos.Width / 2, 290, 950, 370);
				lpcFont->draw(lstr, recti(651 - pos.Width / 2, 291, 950, 370), 0xff000000);
				lpcFont->draw(lstr, loc, 0xffffffff);
				if(dInfo.vic_string && (showcardcode == 1 || showcardcode == 2)) {
					driver->draw2DRectangle(0xa0000000, recti(540, 320, 800, 340));
					guiFont->draw(dInfo.vic_string, recti(502, 321, 840, 340), 0xff000000, true, true);
					guiFont->draw(dInfo.vic_string, recti(500, 320, 840, 340), 0xffffffff, true, true);
				}
			} else if(showcardp < showcarddif + 10) {
				int alpha = ((showcarddif + 10 - showcardp) * 25) << 24;
				lpcFont->draw(lstr, recti(651 - pos.Width / 2 + (showcardp - showcarddif) * 40, 291, 950, 370), alpha);
				lpcFont->draw(lstr, recti(650 - pos.Width / 2 + (showcardp - showcarddif) * 40, 290, 950, 370), alpha | 0xffffff);
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
		driver->drawVertexPrimitiveList(&matManager.vArrow[attack_sv], 12, matManager.iArrow, 10, EVT_STANDARD, EPT_TRIANGLE_STRIP);
		attack_sv += 4;
		if (attack_sv > 28)
			attack_sv = 0;
	}
	bool showChat = true;
	if(hideChat) {
	    showChat = false;
	    hideChatTimer = 10;
	} else if(hideChatTimer > 0) {
	    showChat = false;
	    hideChatTimer--;
	}
	int maxChatLines = mainGame->dInfo.isStarted ? 5 : 8;
	for(int i = 0; i < maxChatLines; ++i) {
		static unsigned int chatColor[] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff8080ff, 0xffff4040, 0xffff4040,
		                                   0xffff4040, 0xff40ff40, 0xff4040ff, 0xff40ffff, 0xffff40ff, 0xffffff40, 0xffffffff, 0xff808080, 0xff404040};
		if(chatTiming[i]) {
			chatTiming[i]--;
			if(!showChat && i > 2)
				continue;
			int w = textFont->getDimension(chatMsg[i].c_str()).Width;
			driver->draw2DRectangle(recti(305, 596 - 20 * i, 307 + w, 616 - 20 * i), 0xa0000000, 0xa0000000, 0xa0000000, 0xa0000000);
			textFont->draw(chatMsg[i].c_str(), rect<s32>(305, 595 - 20 * i, 1020, 615 - 20 * i), 0xff000000, false, false);
			textFont->draw(chatMsg[i].c_str(), rect<s32>(306, 596 - 20 * i, 1021, 616 - 20 * i), chatColor[chatType[i]], false, false);
		}
	}
}
void Game::DrawBackImage(irr::video::ITexture* texture) {
	if(!texture)
		return;
	driver->draw2DImage(texture, recti(0, 0, 1024, 640), recti(0, 0, texture->getOriginalSize().Width, texture->getOriginalSize().Height));
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
	fu.signalAction = 0;
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
	if(win == wCardDisplay) {
		for(int i = 0; i < 5; ++i)
			btnCardDisplay[i]->setDrawImage(false);
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
		dField.conti_selecting = false;
	}
	if(win == wCardDisplay) {
		for(int i = 0; i < 5; ++i)
			btnCardDisplay[i]->setDrawImage(false);
	}
	fadingList.push_back(fu);
}
void Game::PopupElement(irr::gui::IGUIElement * element, int hideframe) {
	element->getParent()->bringToFront(element);
	if(!mainGame->is_building)
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
	if(mainGame->cbLimit->getSelected() >= 4) {
		switch(cp->second.ot) {
		case 1:
			driver->draw2DImage(imageManager.tOT, recti(pos.X + 7, pos.Y + 50, pos.X + 37, pos.Y + 65), recti(0, 0, 128, 64), 0, 0, true);
			break;
		case 2:
			driver->draw2DImage(imageManager.tOT, recti(pos.X + 7, pos.Y + 50, pos.X + 37, pos.Y + 65), recti(0, 64, 128, 128), 0, 0, true);
			break;
		}
	}
}
void Game::DrawDeckBd() {
	wchar_t textBuffer[64];
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
	for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i) {
		DrawThumb(deckManager.current_deck.main[i], position2di(314 + (i % lx) * dx, 164 + (i / lx) * 68), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 1 && deckBuilder.hovered_seq == (int)i)
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
	for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i) {
		DrawThumb(deckManager.current_deck.extra[i], position2di(314 + i * dx, 466), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 2 && deckBuilder.hovered_seq == (int)i)
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
	for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i) {
		DrawThumb(deckManager.current_deck.side[i], position2di(314 + i * dx, 564), deckBuilder.filterList);
		if(deckBuilder.hovered_pos == 3 && deckBuilder.hovered_seq == (int)i)
			driver->draw2DRectangleOutline(recti(313 + i * dx, 563, 359 + i * dx, 629));
	}
	//search result
	driver->draw2DRectangle(recti(805, 137, 915, 157), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(804, 136, 915, 157));
	textFont->draw(dataManager.GetSysString(1333), recti(809, 136, 914, 156), 0xff000000, false, true);
	textFont->draw(dataManager.GetSysString(1333), recti(810, 137, 915, 157), 0xffffffff, false, true);
	numFont->draw(deckBuilder.result_string, recti(874, 136, 934, 156), 0xff000000, false, true);
	numFont->draw(deckBuilder.result_string, recti(875, 137, 935, 157), 0xffffffff, false, true);
	driver->draw2DRectangle(recti(805, 160, 1020, 630), 0x400000ff, 0x400000ff, 0x40000000, 0x40000000);
	driver->draw2DRectangleOutline(recti(804, 159, 1020, 630));
	for(size_t i = 0; i < 7 && i + mainGame->scrFilter->getPos() < deckBuilder.results.size(); ++i) {
		code_pointer ptr = deckBuilder.results[i + mainGame->scrFilter->getPos()];
		if(deckBuilder.hovered_pos == 4 && deckBuilder.hovered_seq == (int)i)
			driver->draw2DRectangle(0x80000000, recti(806, 164 + i * 66, 1019, 230 + i * 66));
		DrawThumb(ptr, position2di(810, 165 + i * 66), deckBuilder.filterList);
		if(ptr->second.type & TYPE_MONSTER) {
			myswprintf(textBuffer, L"%ls", dataManager.GetName(ptr->first));
			textFont->draw(textBuffer, recti(859, 164 + i * 66, 955, 185 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 165 + i * 66, 955, 185 + i * 66), 0xffffffff, false, false);
			if(!(ptr->second.type & TYPE_LINK)) {
				wchar_t* form = L"\u2605";
				if(ptr->second.type & TYPE_XYZ) form = L"\u2606";
				myswprintf(textBuffer, L"%ls/%ls %ls%d", dataManager.FormatAttribute(ptr->second.attribute), dataManager.FormatRace(ptr->second.race), form, ptr->second.level);
				textFont->draw(textBuffer, recti(859, 186 + i * 66, 955, 207 + i * 66), 0xff000000, false, false);
				textFont->draw(textBuffer, recti(860, 187 + i * 66, 955, 207 + i * 66), 0xffffffff, false, false);
				if(ptr->second.attack < 0 && ptr->second.defense < 0)
					myswprintf(textBuffer, L"?/?");
				else if(ptr->second.attack < 0)
					myswprintf(textBuffer, L"?/%d", ptr->second.defense);
				else if(ptr->second.defense < 0)
					myswprintf(textBuffer, L"%d/?", ptr->second.attack);
				else myswprintf(textBuffer, L"%d/%d", ptr->second.attack, ptr->second.defense);
			} else {
				myswprintf(textBuffer, L"%ls/%ls LINK-%d", dataManager.FormatAttribute(ptr->second.attribute), dataManager.FormatRace(ptr->second.race), ptr->second.level);
				textFont->draw(textBuffer, recti(859, 186 + i * 66, 955, 207 + i * 66), 0xff000000, false, false);
				textFont->draw(textBuffer, recti(860, 187 + i * 66, 955, 207 + i * 66), 0xffffffff, false, false);
				if(ptr->second.attack < 0)
					myswprintf(textBuffer, L"?/-");
				else myswprintf(textBuffer, L"%d/-", ptr->second.attack);
			}
			if(ptr->second.type & TYPE_PENDULUM) {
				wchar_t scaleBuffer[16];
				myswprintf(scaleBuffer, L" %d/%d", ptr->second.lscale, ptr->second.rscale);
				wcscat(textBuffer, scaleBuffer);
			}
			if((ptr->second.ot & 0x3) == 1)
				wcscat(textBuffer, L" [OCG]");
			else if((ptr->second.ot & 0x3) == 2)
				wcscat(textBuffer, L" [TCG]");
			else if((ptr->second.ot & 0x7) == 4)
				wcscat(textBuffer, L" [Custom]");
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
			else if((ptr->second.ot & 0x7) == 4)
				wcscat(textBuffer, L"[Custom]");
			textFont->draw(textBuffer, recti(859, 208 + i * 66, 955, 229 + i * 66), 0xff000000, false, false);
			textFont->draw(textBuffer, recti(860, 209 + i * 66, 955, 229 + i * 66), 0xffffffff, false, false);
		}
	}
	if(deckBuilder.is_draging) {
		DrawThumb(deckBuilder.draging_pointer, position2di(deckBuilder.dragx - 22, deckBuilder.dragy - 32), deckBuilder.filterList);
	}
}
}
