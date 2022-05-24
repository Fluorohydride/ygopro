#include <fmt/format.h>
#include "game_config.h"
#include <irrlicht.h>
#include "game.h"
#include "materials.h"
#include "client_card.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "CGUITTFont/CGUITTFont.h"
#include "CGUIImageButton/CGUIImageButton.h"
#include "custom_skin_enum.h"
#include "image_manager.h"

namespace ygo {
void Game::DrawSelectionLine(irr::video::S3DVertex vec[4], bool strip, int width, irr::video::SColor color) {
	driver->setMaterial(matManager.mOutLine);
	if(strip && !gGameConfig->dotted_lines) {
		int pattern = linePatternD3D - 14;
		bool swap = false;
		if(linePatternD3D < 15) {
			pattern += 15;
			swap = true;
		}
		auto drawLine = [&](const auto& pos1, const auto& pos2) -> void {
			if(swap)
				driver->draw3DLineW(pos1 + (pos2 - pos1) * (pattern) / 15.0, pos2, color, width);
			else
				driver->draw3DLineW(pos1, pos1 + (pos2 - pos1) * (pattern) / 15.0, color, width);
		};
		drawLine(vec[0].Pos, vec[1].Pos);
		drawLine(vec[1].Pos, vec[3].Pos);
		drawLine(vec[3].Pos, vec[2].Pos);
		drawLine(vec[2].Pos, vec[0].Pos);
	} else {
		std::vector<irr::core::vector3df> pos{ vec[0].Pos, vec[1].Pos, vec[3].Pos, vec[2].Pos };
		driver->draw3DShapeW(pos.data(), pos.size(), color, width, strip ? linePatternGL : 0xffff);
	}
}
void Game::DrawBackGround() {
	static float selFieldAlpha = 255;
	static float selFieldDAlpha = -10;
	//draw field
	//draw field spell card
	driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
	bool drawField = false;
	int field = (dInfo.duel_field == 3 || dInfo.duel_field == 5) ? 0 : 1;
	int tfield = 3;
	switch (dInfo.duel_field) {
		case 1:
		case 2: {
			tfield = 2;
			break;
		}
		case 3: {
			tfield = 0;
			break;
		}
		case 4: {
			tfield = 1;
			break;
		}
	}
	int speed = (dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	if(gGameConfig->draw_field_spell) {
		int fieldcode1 = -1;
		int fieldcode2 = -1;
		if(dField.szone[0][5] && dField.szone[0][5]->position & POS_FACEUP)
			fieldcode1 = dField.szone[0][5]->code;
		if(dField.szone[1][5] && dField.szone[1][5]->position & POS_FACEUP)
			fieldcode2 = dField.szone[1][5]->code;
		int fieldcode = (fieldcode1 > 0) ? fieldcode1 : fieldcode2;
		if(fieldcode1 > 0 && fieldcode2 > 0 && fieldcode1 != fieldcode2) {
			irr::video::ITexture* texture = imageManager.GetTextureField(fieldcode1);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell1[speed], 4, matManager.iRectangle, 2);
			}
			texture = imageManager.GetTextureField(fieldcode2);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell2[speed], 4, matManager.iRectangle, 2);
			}
		} else if(fieldcode > 0) {
			irr::video::ITexture* texture = imageManager.GetTextureField(fieldcode);
			if(texture) {
				drawField = true;
				matManager.mTexture.setTexture(0, texture);
				driver->setMaterial(matManager.mTexture);
				driver->drawVertexPrimitiveList(matManager.vFieldSpell[speed], 4, matManager.iRectangle, 2);
			}
		}
	}
	matManager.mTexture.setTexture(0, drawField ? imageManager.tFieldTransparent[speed][tfield] : imageManager.tField[speed][tfield]);
	driver->setMaterial(matManager.mTexture);
	driver->drawVertexPrimitiveList(matManager.vField, 4, matManager.iRectangle, 2);
	driver->setMaterial(matManager.mBackLine);
	//select field
	if((dInfo.curMsg == MSG_SELECT_PLACE || dInfo.curMsg == MSG_SELECT_DISFIELD || dInfo.curMsg == MSG_HINT) && dField.selectable_field) {
		irr::video::SColor outline_color = skin::DUELFIELD_SELECTABLE_FIELD_OUTLINE_VAL;
		uint32_t filter = 0x1;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldMzone[0][i], !(dField.selected_field & filter), 2, outline_color);
		}
		filter = 0x100;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldSzone[0][i][field][speed], !(dField.selected_field & filter), 2, outline_color);
		}
		filter = 0x10000;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldMzone[1][i], !(dField.selected_field & filter), 2, outline_color);
		}
		filter = 0x1000000;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.selectable_field & filter)
				DrawSelectionLine(matManager.vFieldSzone[1][i][field][speed], !(dField.selected_field & filter), 2, outline_color);
		}
	}
	//disabled field
	{
		irr::video::SColor disabled_color = skin::DUELFIELD_DISABLED_FIELD_COLOR_VAL;
		uint32_t filter = 0x1;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldMzone[0][i][0].Pos, matManager.vFieldMzone[0][i][3].Pos, disabled_color);
				driver->draw3DLine(matManager.vFieldMzone[0][i][1].Pos, matManager.vFieldMzone[0][i][2].Pos, disabled_color);
			}
		}
		filter = 0x100;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldSzone[0][i][field][speed][0].Pos, matManager.vFieldSzone[0][i][field][speed][3].Pos, disabled_color);
				driver->draw3DLine(matManager.vFieldSzone[0][i][field][speed][1].Pos, matManager.vFieldSzone[0][i][field][speed][2].Pos, disabled_color);
			}
		}
		filter = 0x10000;
		for (int i = 0; i < 7; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldMzone[1][i][0].Pos, matManager.vFieldMzone[1][i][3].Pos, disabled_color);
				driver->draw3DLine(matManager.vFieldMzone[1][i][1].Pos, matManager.vFieldMzone[1][i][2].Pos, disabled_color);
			}
		}
		filter = 0x1000000;
		for (int i = 0; i < 8; ++i, filter <<= 1) {
			if (dField.disabled_field & filter) {
				driver->draw3DLine(matManager.vFieldSzone[1][i][field][speed][0].Pos, matManager.vFieldSzone[1][i][field][speed][3].Pos, disabled_color);
				driver->draw3DLine(matManager.vFieldSzone[1][i][field][speed][1].Pos, matManager.vFieldSzone[1][i][field][speed][2].Pos, disabled_color);
			}
		}
	}
	auto setAlpha = [](irr::video::SMaterial& material, const irr::video::SColor& color) {
		uint32_t endalpha = std::round(color.getAlpha() * (selFieldAlpha - 5.0) * (0.005));
		material.DiffuseColor = endalpha << 24;
		material.AmbientColor = color;
	};
	//current sel
	if (dField.hovered_location != 0 && dField.hovered_location != 2 && dField.hovered_location != POSITION_HINT
		&& !(dInfo.duel_field < 4 && dField.hovered_location == LOCATION_MZONE && dField.hovered_sequence > 4)
		&& !(dInfo.duel_field != 3 && dInfo.duel_field != 5 && dField.hovered_location == LOCATION_SZONE && dField.hovered_sequence > 5)) {
		setAlpha(matManager.mLinkedField, skin::DUELFIELD_LINKED_VAL);
		setAlpha(matManager.mMutualLinkedField, skin::DUELFIELD_MUTUAL_LINKED_VAL);
		selFieldAlpha += selFieldDAlpha * (float)delta_time * 60.0f / 1000.0f;
		if(selFieldAlpha <= 5) {
			selFieldAlpha = 5;
			selFieldDAlpha = 10;
		}
		if(selFieldAlpha >= 205) {
			selFieldAlpha = 205;
			selFieldDAlpha = -10;
		}
		setAlpha(matManager.mSelField, skin::DUELFIELD_HOVERED_VAL);
		irr::video::S3DVertex *vertex = nullptr;
		if (dField.hovered_location == LOCATION_DECK)
			vertex = matManager.vFieldDeck[dField.hovered_controler][speed];
		else if (dField.hovered_location == LOCATION_MZONE) {
			vertex = matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence];
			ClientCard* pcard = dField.mzone[dField.hovered_controler][dField.hovered_sequence];
			if(pcard && (pcard->type & TYPE_LINK) && (pcard->position & POS_FACEUP)) {
				DrawLinkedZones(pcard);
			}
		} else if(dField.hovered_location == LOCATION_SZONE) {
			vertex = matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence][field][speed];
			ClientCard* pcard = dField.szone[dField.hovered_controler][dField.hovered_sequence];
			if(pcard && (pcard->type & TYPE_LINK) && (pcard->position & POS_FACEUP))
				DrawLinkedZones(pcard);
		}
		else if (dField.hovered_location == LOCATION_GRAVE)
			vertex = matManager.vFieldGrave[dField.hovered_controler][field][speed];
		else if (dField.hovered_location == LOCATION_REMOVED)
			vertex = matManager.vFieldRemove[dField.hovered_controler][field][speed];
		else if (dField.hovered_location == LOCATION_EXTRA)
			vertex = matManager.vFieldExtra[dField.hovered_controler][speed];
		if(!vertex)
			return;
		driver->setMaterial(matManager.mSelField);
		driver->drawVertexPrimitiveList(vertex, 4, matManager.iRectangle, 2);
	}
}
void Game::DrawLinkedZones(ClientCard* pcard) {
	auto CheckMutual = [&](ClientCard* pcard, int mark)->bool {
		driver->setMaterial(matManager.mLinkedField);
		if(pcard && pcard->type & TYPE_LINK && pcard->link_marker & mark) {
			driver->setMaterial(matManager.mMutualLinkedField);
			return true;
		}
		return false;
	};
	const int mark = pcard->link_marker;
	ClientCard* pcard2;
	const uint32_t speed = (dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	if(dField.hovered_location == LOCATION_SZONE) {
		int field = (dInfo.duel_field == 3 || dInfo.duel_field == 5) ? 0 : 1;
		if(dField.hovered_sequence > 4)
			return;
		if(mark & LINK_MARKER_TOP_LEFT && dField.hovered_sequence > (0 + speed)) {
			pcard2 = dField.mzone[dField.hovered_controler][dField.hovered_sequence - 1];
			CheckMutual(pcard2, LINK_MARKER_BOTTOM_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence - 1], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_TOP) {
			pcard2 = dField.mzone[dField.hovered_controler][dField.hovered_sequence];
			CheckMutual(pcard2, LINK_MARKER_BOTTOM);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_TOP_RIGHT && dField.hovered_sequence < (4 - speed)) {
			pcard2 = dField.mzone[dField.hovered_controler][dField.hovered_sequence + 1];
			CheckMutual(pcard2, LINK_MARKER_BOTTOM_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence + 1], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_LEFT && dField.hovered_sequence >(0 + speed)) {
			pcard2 = dField.szone[dField.hovered_controler][dField.hovered_sequence - 1];
			CheckMutual(pcard2, LINK_MARKER_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence - 1][field][speed], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_RIGHT && dField.hovered_sequence < (4 - speed)) {
			pcard2 = dField.szone[dField.hovered_controler][dField.hovered_sequence + 1];
			CheckMutual(pcard2, LINK_MARKER_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence + 1][field][speed], 4, matManager.iRectangle, 2);
		}
		return;
	}
	if (dField.hovered_sequence < 5) {
		if (mark & LINK_MARKER_LEFT && dField.hovered_sequence > (0 + speed)) {
			pcard2 = dField.mzone[dField.hovered_controler][dField.hovered_sequence - 1];
			CheckMutual(pcard2, LINK_MARKER_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence - 1], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_RIGHT && dField.hovered_sequence < (4 - speed)) {
			pcard2 = dField.mzone[dField.hovered_controler][dField.hovered_sequence + 1];
			CheckMutual(pcard2, LINK_MARKER_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][dField.hovered_sequence + 1], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_BOTTOM_LEFT && dField.hovered_sequence > (0 + speed)) {
			pcard2 = dField.szone[dField.hovered_controler][dField.hovered_sequence - 1];
			if(CheckMutual(pcard2, LINK_MARKER_TOP_RIGHT))
				driver->drawVertexPrimitiveList(&matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence - 1], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_BOTTOM_RIGHT && dField.hovered_sequence < (4 - speed)) {
			pcard2 = dField.szone[dField.hovered_controler][dField.hovered_sequence + 1];
			if(CheckMutual(pcard2, LINK_MARKER_TOP_LEFT))
				driver->drawVertexPrimitiveList(&matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence + 1], 4, matManager.iRectangle, 2);
		}
		if(mark & LINK_MARKER_BOTTOM) {
			pcard2 = dField.szone[dField.hovered_controler][dField.hovered_sequence];
			if(CheckMutual(pcard2, LINK_MARKER_TOP))
				driver->drawVertexPrimitiveList(&matManager.vFieldSzone[dField.hovered_controler][dField.hovered_sequence], 4, matManager.iRectangle, 2);
		}
		if (dInfo.duel_field >= 4) {
			if ((mark & LINK_MARKER_TOP_LEFT && dField.hovered_sequence == 2)
				|| (mark & LINK_MARKER_TOP && dField.hovered_sequence == 1)
				|| (mark & LINK_MARKER_TOP_RIGHT && dField.hovered_sequence == 0)) {
				int mark = (dField.hovered_sequence == 2) ? LINK_MARKER_BOTTOM_RIGHT : (dField.hovered_sequence == 1) ? LINK_MARKER_BOTTOM : LINK_MARKER_BOTTOM_LEFT;
				pcard2 = dField.mzone[dField.hovered_controler][5];
				if (!pcard2) {
					pcard2 = dField.mzone[1 - dField.hovered_controler][6];
					mark = (dField.hovered_sequence == 2) ? LINK_MARKER_TOP_LEFT : (dField.hovered_sequence == 1) ? LINK_MARKER_TOP : LINK_MARKER_TOP_RIGHT;
				}
				CheckMutual(pcard2, mark);
				driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][5], 4, matManager.iRectangle, 2);
			}
			if ((mark & LINK_MARKER_TOP_LEFT && dField.hovered_sequence == 4)
				|| (mark & LINK_MARKER_TOP && dField.hovered_sequence == 3)
				|| (mark & LINK_MARKER_TOP_RIGHT && dField.hovered_sequence == 2)) {
				int mark = (dField.hovered_sequence == 4) ? LINK_MARKER_BOTTOM_RIGHT : (dField.hovered_sequence == 3) ? LINK_MARKER_BOTTOM : LINK_MARKER_BOTTOM_LEFT;
				pcard2 = dField.mzone[dField.hovered_controler][6];
				if (!pcard2) {
					pcard2 = dField.mzone[1 - dField.hovered_controler][5];
					mark = (dField.hovered_sequence == 4) ? LINK_MARKER_TOP_LEFT : (dField.hovered_sequence == 3) ? LINK_MARKER_TOP : LINK_MARKER_TOP_RIGHT;
				}
				CheckMutual(pcard2, mark);
				driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][6], 4, matManager.iRectangle, 2);
			}
		}
	} else {
		int swap = (dField.hovered_sequence == 5) ? 0 : 2;
		if (mark & LINK_MARKER_BOTTOM_LEFT && !(speed && swap == 0)) {
			pcard2 = dField.mzone[dField.hovered_controler][0 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][0 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_BOTTOM) {
			pcard2 = dField.mzone[dField.hovered_controler][1 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][1 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_BOTTOM_RIGHT && !(speed && swap == 2)) {
			pcard2 = dField.mzone[dField.hovered_controler][2 + swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[dField.hovered_controler][2 + swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP_LEFT && !(speed && swap == 0)) {
			pcard2 = dField.mzone[1 - dField.hovered_controler][4 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_LEFT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][4 - swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP) {
			pcard2 = dField.mzone[1 - dField.hovered_controler][3 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][3 - swap], 4, matManager.iRectangle, 2);
		}
		if (mark & LINK_MARKER_TOP_RIGHT && !(speed && swap == 2)) {
			pcard2 = dField.mzone[1 - dField.hovered_controler][2 - swap];
			CheckMutual(pcard2, LINK_MARKER_TOP_RIGHT);
			driver->drawVertexPrimitiveList(&matManager.vFieldMzone[1 - dField.hovered_controler][2 - swap], 4, matManager.iRectangle, 2);
		}
	}
}
void Game::DrawCards() {
	for(auto& pcard : dField.overlay_cards)
		DrawCard(pcard);
	for(int p = 0; p < 2; ++p) {
		for(auto& pcard : dField.mzone[p])
			if(pcard)
				DrawCard(pcard);
		for(auto& pcard : dField.szone[p])
			if(pcard)
				DrawCard(pcard);
		for(auto& pcard : dField.deck[p])
			DrawCard(pcard);
		for(auto& pcard : dField.hand[p])
			DrawCard(pcard);
		for(auto& pcard : dField.grave[p])
			DrawCard(pcard);
		for(auto& pcard : dField.remove[p])
			DrawCard(pcard);
		for(auto& pcard : dField.extra[p])
			DrawCard(pcard);
		if(dField.skills[p])
			DrawCard(dField.skills[p]);
	}
}
void Game::DrawCard(ClientCard* pcard) {
	if(pcard->aniFrame > 0) {
		uint32_t movetime = std::min<uint32_t>(delta_time, pcard->aniFrame);
		if(pcard->is_moving) {
			pcard->curPos += (pcard->dPos * movetime);
			pcard->curRot += (pcard->dRot * movetime);
			pcard->mTransform.setTranslation(pcard->curPos);
			pcard->mTransform.setRotationRadians(pcard->curRot);
		}
		if(pcard->is_fading)
			pcard->curAlpha += pcard->dAlpha * movetime;
		pcard->aniFrame -= movetime;
		if(pcard->aniFrame <= 0) {
			pcard->aniFrame = 0;
			pcard->is_moving = false;
			pcard->is_fading = false;
			if(pcard->refresh_on_stop)
				pcard->UpdateDrawCoordinates(true);
			pcard->refresh_on_stop = false;
		}
	}
	matManager.mCard.AmbientColor = 0xffffffff;
	matManager.mCard.DiffuseColor = ((int)std::round(pcard->curAlpha) << 24) | 0xffffff;
	driver->setTransform(irr::video::ETS_WORLD, pcard->mTransform);
	auto m22 = pcard->mTransform(2, 2);
	if(m22 > -0.99 || pcard->is_moving) {
		matManager.mCard.setTexture(0, imageManager.GetTextureCard(pcard->code, imgType::ART));
		driver->setMaterial(matManager.mCard);
		driver->drawVertexPrimitiveList(matManager.vCardFront, 4, matManager.iRectangle, 2);
	}
	if(m22 < 0.99 || pcard->is_moving) {
		auto txt = imageManager.GetTextureCard(pcard->cover, imgType::COVER);
		if(txt == imageManager.tCover[0]) {
			matManager.mCard.setTexture(0, imageManager.tCover[pcard->controler]);
		} else {
			matManager.mCard.setTexture(0, imageManager.GetTextureCard(pcard->cover, imgType::COVER));
		}
		driver->setMaterial(matManager.mCard);
		driver->drawVertexPrimitiveList(matManager.vCardBack, 4, matManager.iRectangle, 2);
	}
	if(pcard->is_selectable && (pcard->location & 0xe)) {
		irr::video::SColor outline_color = skin::DUELFIELD_SELECTABLE_CARD_OUTLINE_VAL;
		if((pcard->location == LOCATION_HAND && pcard->code) || ((pcard->location & 0xc) && (pcard->position & POS_FACEUP)))
			DrawSelectionLine(matManager.vCardOutline, !pcard->is_selected, 2, outline_color);
		else
			DrawSelectionLine(matManager.vCardOutliner, !pcard->is_selected, 2, outline_color);
	}
	if(pcard->is_highlighting) {
		irr::video::SColor outline_color = skin::DUELFIELD_HIGHLIGHTING_CARD_OUTLINE_VAL;
		if((pcard->location == LOCATION_HAND && pcard->code) || ((pcard->location & 0xc) && (pcard->position & POS_FACEUP)))
			DrawSelectionLine(matManager.vCardOutline, true, 2, outline_color);
		else
			DrawSelectionLine(matManager.vCardOutliner, true, 2, outline_color);
	}
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
	} else if((pcard->status & (STATUS_DISABLED | STATUS_FORBIDDEN))
		&& (pcard->location & LOCATION_ONFIELD) && (pcard->position & POS_FACEUP)) {
		matManager.mTexture.setTexture(0, imageManager.tNegated);
		driver->setMaterial(matManager.mTexture);
		driver->drawVertexPrimitiveList(matManager.vNegate, 4, matManager.iRectangle, 2);
	}
	if(pcard->is_moving)
		return;
	if(pcard->cmdFlag & COMMAND_ATTACK) {
		matManager.mTexture.setTexture(0, imageManager.tAttack);
		driver->setMaterial(matManager.mTexture);
		irr::core::matrix4 atk;
		atk.setTranslation(pcard->curPos + irr::core::vector3df(0, (pcard->controler == 0 ? -1 : 1) * (atkdy / 4.0f + 0.35f), 0.05f));
		atk.setRotationRadians(irr::core::vector3df(0, 0, pcard->controler == 0 ? 0 : irr::core::PI));
		driver->setTransform(irr::video::ETS_WORLD, atk);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
	}
}
template<typename T>
inline void DrawShadowTextPos(irr::gui::CGUITTFont* font, const T& text, const irr::core::recti& shadowposition, const irr::core::recti& mainposition,
					   irr::video::SColor color = 0xffffffff, irr::video::SColor shadowcolor = 0xff000000, bool hcenter = false, bool vcenter = false, const irr::core::recti* clip = nullptr) {
	font->drawustring(text, shadowposition, shadowcolor, hcenter, vcenter, clip);
	font->drawustring(text, mainposition, color, hcenter, vcenter, clip);
}
//We don't want multiple function signatures per argument combination
#if !defined(_MSC_VER) && !defined(__forceinline)
#define __forceinline __attribute__((always_inline)) inline
#endif
template<typename T, typename... Args>
__forceinline void DrawShadowText(irr::gui::CGUITTFont* font, const T& text, const irr::core::recti& shadowposition, const irr::core::recti& padding, Args&&... args) {
	const irr::core::recti position(shadowposition.UpperLeftCorner.X + padding.UpperLeftCorner.X, shadowposition.UpperLeftCorner.Y + padding.UpperLeftCorner.Y,
									shadowposition.LowerRightCorner.X + padding.LowerRightCorner.X, shadowposition.LowerRightCorner.Y + padding.LowerRightCorner.Y);
	DrawShadowTextPos(font, text, shadowposition, position, std::forward<Args>(args)...);
}
void Game::DrawMisc() {
	const float twoPI = 2.0f * irr::core::PI;
	static float act_rot = 0.0f;
	//pre expanded version of setRotationRadians, we're only setting the z value, saves computations
	auto SetZRotation = [](irr::core::matrix4& mat) {
		mat[2] = mat[6] = mat[8] = mat[9] = 0;
		mat[10] = 1;
		const auto _cos = std::cos(act_rot);
		const auto _sin = std::sin(act_rot);
		mat[0] = mat[5] = _cos;
		mat[1] = _sin;
		mat[4] = -_sin;
	};
	const int field = (dInfo.duel_field == 3 || dInfo.duel_field == 5) ? 0 : 1;
	const int speed = (dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	irr::core::matrix4 im, ic, it;
	act_rot += (1.2f / 1000.0f) * delta_time;
	if(act_rot >= twoPI) {
		act_rot -= twoPI;
		//double branch to account for random instances where the value increases too much
		if(act_rot >= twoPI) {
			act_rot = fmod(act_rot, twoPI);
		}
	}
	SetZRotation(im);
	matManager.mTexture.setTexture(0, imageManager.tAct);
	driver->setMaterial(matManager.mTexture);

	auto drawact = [this, &im](irr::video::S3DVertex vertex[4], float zval) {
		im.setTranslation(irr::core::vector3df((vertex[0].Pos.X + vertex[1].Pos.X) / 2,
			(vertex[0].Pos.Y + vertex[2].Pos.Y) / 2, zval));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	};

	int pzseq = dInfo.duel_field == 4 ? (speed) ? 1 : 0 : 6;
	for(int p = 0; p < 2; p++) {
		if(dField.deck_act[p])
			drawact(matManager.vFieldDeck[p][speed], dField.deck[p].size() * 0.01f + 0.02f);
		if(dField.grave_act[p])
			drawact(matManager.vFieldGrave[p][field][speed], dField.grave[p].size() * 0.01f + 0.02f);
		if(dField.remove_act[p])
			drawact(matManager.vFieldRemove[p][field][speed], dField.remove[p].size() * 0.01f + 0.02f);
		if(dField.extra_act[p])
			drawact(matManager.vFieldExtra[p][speed], dField.extra[p].size() * 0.01f + 0.02f);
		if(dField.pzone_act[p])
			drawact(matManager.vFieldSzone[p][pzseq][field][speed], 0.03f);
	}

	if(dField.conti_act) {
		im.setTranslation(irr::core::vector3df((matManager.vFieldContiAct[speed][0].X + matManager.vFieldContiAct[speed][1].X) / 2,
			(matManager.vFieldContiAct[speed][0].Y + matManager.vFieldContiAct[speed][2].Y) / 2, 0.03f));
		driver->setTransform(irr::video::ETS_WORLD, im);
		driver->drawVertexPrimitiveList(matManager.vActivate, 4, matManager.iRectangle, 2);
	}

	matManager.mTRTexture.AmbientColor = skin::DUELFIELD_CHAIN_COLOR_VAL;
	auto setCoords = [&](int i) {
		const auto div = i / 5;
		const auto mod = i % 5;
		matManager.vChainNum[0].TCoords = irr::core::vector2df(0.19375f * mod, 0.2421875f * div);
		matManager.vChainNum[1].TCoords = irr::core::vector2df(0.19375f * (mod + 1), 0.2421875f * div);
		matManager.vChainNum[2].TCoords = irr::core::vector2df(0.19375f * mod, 0.2421875f * (div + 1));
		matManager.vChainNum[3].TCoords = irr::core::vector2df(0.19375f * (mod + 1), 0.2421875f * (div + 1));
	};
	for(size_t i = 0; i < dField.chains.size(); ++i) {
		const auto& chain = dField.chains[i];
		if(chain.solved)
			break;
		matManager.mTRTexture.setTexture(0, imageManager.tChain);
		SetZRotation(ic);
		ic.setTranslation(chain.chain_pos);
		driver->setMaterial(matManager.mTRTexture);
		driver->setTransform(irr::video::ETS_WORLD, ic);
		driver->drawVertexPrimitiveList(matManager.vSymbol, 4, matManager.iRectangle, 2);
		it.setScale(0.6f);
		it.setTranslation(chain.chain_pos);
		matManager.mTRTexture.setTexture(0, imageManager.tNumber);
		setCoords(i);
		driver->setMaterial(matManager.mTRTexture);
		driver->setTransform(irr::video::ETS_WORLD, it);
		driver->drawVertexPrimitiveList(matManager.vChainNum, 4, matManager.iRectangle, 2);
	}
	//lp bar
	const auto& self = dInfo.isTeam1 ? dInfo.selfnames : dInfo.opponames;
	const auto& oppo = dInfo.isTeam1 ? dInfo.opponames : dInfo.selfnames;
	const auto rectpos = ((dInfo.turn % 2 && dInfo.isFirst) || (!(dInfo.turn % 2) && !dInfo.isFirst)) ?
						Resize(327, 8, 630, 51 + (23 * (self.size() - 1))) :
						Resize(689, 8, 991, 51 + (23 * (oppo.size() - 1)));
	driver->draw2DRectangle(skin::DUELFIELD_TURNPLAYER_COLOR_VAL, rectpos);
	driver->draw2DRectangleOutline(rectpos, skin::DUELFIELD_TURNPLAYER_OUTLINE_COLOR_VAL);
	driver->draw2DImage(imageManager.tLPFrame, Resize(330, 10, 629, 30), irr::core::recti(0, 0, 200, 20), 0, 0, true);
	driver->draw2DImage(imageManager.tLPFrame, Resize(691, 10, 990, 30), irr::core::recti(0, 0, 200, 20), 0, 0, true);

#define SKCOLOR(what) skin::LPBAR_##what##_VAL
#define RECTCOLOR(what) SKCOLOR(what##_TOP_LEFT), SKCOLOR(what##_TOP_RIGHT), SKCOLOR(what##_BOTTOM_LEFT), SKCOLOR(what##_BOTTOM_RIGHT)
#define	DRAWRECT(rect_pos,what,clip) do { driver->draw2DRectangleClip(rect_pos, RECTCOLOR(what),nullptr,clip); } while(0)
	if(dInfo.lp[0]) {
		const auto rectpos = Resize(335, 12, 625, 28);
		if(dInfo.lp[0] < dInfo.startlp) {
			auto cliprect = Resize(335, 12, 335 + 290 * dInfo.lp[0] / dInfo.startlp, 28);
			DRAWRECT(rectpos, 1, &cliprect);
		} else {
			DRAWRECT(rectpos, 1, nullptr);
		}
	}
	if(dInfo.lp[1] > 0) {
		const auto rectpos = Resize(696, 12, 986, 28);
		if(dInfo.lp[1] < dInfo.startlp) {
			auto cliprect = Resize(986 - 290 * dInfo.lp[1] / dInfo.startlp, 12, 986, 28);
			DRAWRECT(rectpos, 2, &cliprect);
		} else {
			DRAWRECT(rectpos, 2, nullptr);
		}
	}
	
	if(lpframe > 0 && delta_frames) {
		dInfo.lp[lpplayer] -= lpd * delta_frames;
		dInfo.strLP[lpplayer] = fmt::to_wstring(std::max(0, dInfo.lp[lpplayer]));
		lpcalpha -= 0x19 * delta_frames;
		lpframe -= delta_frames;
	}
	if(lpcstring.size()) {
		if(lpplayer == 0)
			DrawShadowText(lpcFont, lpcstring, Resize(400, 470, 920, 520), Resize(0, 2, 2, 0), (lpcalpha << 24) | lpccolor, (lpcalpha << 24) | 0x00ffffff, true);
		else
			DrawShadowText(lpcFont, lpcstring, Resize(400, 160, 920, 210), Resize(0, 2, 2, 0), (lpcalpha << 24) | lpccolor, (lpcalpha << 24) | 0x00ffffff, true);
	}

#undef SKCOLOR
#define SKCOLOR(what) skin::TIMEBAR_##what##_VAL

	if(!dInfo.isReplay && !dInfo.isSingleMode && dInfo.player_type < 7 && dInfo.time_limit) {
		auto rectpos = Resize(525, 34, 625, 44);
		auto cliprect = Resize(525, 34, 525 + dInfo.time_left[0] * 100 / dInfo.time_limit, 44);
		DRAWRECT(rectpos, 1, &cliprect);
		driver->draw2DRectangleOutline(rectpos, skin::TIMEBAR_1_OUTLINE_VAL);
		rectpos = Resize(695, 34, 795, 44);
		cliprect = Resize(795 - dInfo.time_left[1] * 100 / dInfo.time_limit, 34, 795, 44);
		DRAWRECT(rectpos, 2, &cliprect);
		driver->draw2DRectangleOutline(rectpos, skin::TIMEBAR_2_OUTLINE_VAL);
	}

	DrawShadowText(numFont, dInfo.strLP[0], Resize(330, 11, 629, 29), Resize(0, 1, 2, 0), skin::DUELFIELD_LP_1_VAL, 0xff000000, true, true);
	DrawShadowText(numFont, dInfo.strLP[1], Resize(691, 11, 990, 29), Resize(0, 1, 2, 0), skin::DUELFIELD_LP_2_VAL, 0xff000000, true, true);

	irr::core::recti p1size = Resize(335, 31, 629, 50);
	irr::core::recti p2size = Resize(986, 31, 986, 50);
	int i = 0;
	for(const auto& player : self) {
		if(i++== dInfo.current_player[0])
			textFont->drawustring(player, p1size, 0xffffffff, false, false, 0);
		else
			textFont->drawustring(player, p1size, 0xff808080, false, false, 0);
		p1size += irr::core::vector2di{ 0, p1size.getHeight() + ResizeY(4) };
	}
	i = 0;
	const auto basecorner = p2size.UpperLeftCorner.X;
	for(const auto& player : oppo) {
		const irr::core::ustring utext(player);
		auto cld = textFont->getDimensionustring(utext);
		p2size.UpperLeftCorner.X = basecorner - cld.Width;
		if(i++ == dInfo.current_player[1])
			textFont->drawustring(utext, p2size, 0xffffffff, false, false, 0);
		else
			textFont->drawustring(utext, p2size, 0xff808080, false, false, 0);
		p2size += irr::core::vector2di{ 0, p2size.getHeight() + ResizeY(4) };
	}
	/*driver->draw2DRectangle(Resize(632, 10, 688, 30), 0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
	driver->draw2DRectangle(Resize(632, 30, 688, 50), 0xffffffff, 0xffffffff, 0x00000000, 0x00000000);*/
	DrawShadowText(lpcFont, gDataManager->GetNumString(dInfo.turn), Resize(635, 5, 685, 40), Resize(0, 0, 2, 0), skin::DUELFIELD_TURN_COUNT_VAL, 0x80000000, true);
#undef DRAWRECT
#undef LPCOLOR
#undef SKCOLOR

	ClientCard* pcard;
	int seq = (dInfo.duel_field != 4) ? 6 : (dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	int increase = (dInfo.duel_field != 4) ? 1 : (dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 2 : 4;
	for (int p = 0, seq2 = seq; p < 2; ++p, seq2 = seq) {
		for (int i = 0; i < 7; ++i) {
			pcard = dField.mzone[p][i];
			if (pcard && pcard->code != 0 && (p == 0 || (pcard->position & POS_FACEUP)))
				DrawStatus(pcard);
		}
		for (int i = 0; i < 2; i++, seq2 += increase) {
			pcard = dField.szone[p][seq2];
			if (pcard && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget)
				DrawPendScale(pcard);
		}
		if (dField.extra[p].size()) {
			const auto str = (dField.extra_p_count[p]) ? fmt::format(L"{}({})", dField.extra[p].size(), dField.extra_p_count[p]) : fmt::format(L"{}", dField.extra[p].size());
			DrawStackIndicator(str, matManager.vFieldExtra[p][speed], (p == 1));
		}
		if (dField.deck[p].size())
			DrawStackIndicator(gDataManager->GetNumString(dField.deck[p].size()), matManager.vFieldDeck[p][speed], (p == 1));
		if (dField.grave[p].size())
			DrawStackIndicator(gDataManager->GetNumString(dField.grave[p].size()), matManager.vFieldGrave[p][field][speed], (p == 1));
		if (dField.remove[p].size())
			DrawStackIndicator(gDataManager->GetNumString(dField.remove[p].size()), matManager.vFieldRemove[p][field][speed], (p == 1));
	}
}
/*
Draws the stats of a card based on its relative position
*/
void Game::DrawStatus(ClientCard* pcard) {
	auto getcoords = [collisionmanager=device->getSceneManager()->getSceneCollisionManager()](const irr::core::vector3df& pos3d) {
		return collisionmanager->getScreenCoordinatesFrom3DPosition(pos3d);
	};
	int x1, y1, x2, y2;
	if (pcard->controler == 0) {
		auto coords = getcoords({ pcard->curPos.X, (0.39f + pcard->curPos.Y), pcard->curPos.Z });
		x1 = coords.X;
		y1 = coords.Y;
		coords = getcoords({ (pcard->curPos.X - 0.48f), (pcard->curPos.Y - 0.66f), pcard->curPos.Z });
		x2 = coords.X;
		y2 = coords.Y;
	} else {
		auto coords = getcoords({ pcard->curPos.X, (pcard->curPos.Y - 0.66f), pcard->curPos.Z });
		x1 = coords.X;
		y1 = coords.Y;
		coords = getcoords({ (pcard->curPos.X - 0.48f), (0.39f + pcard->curPos.Y), pcard->curPos.Z });
		x2 = coords.X;
		y2 = coords.Y;
	}

	auto GetAtkColor = [&pcard] {
		if(pcard->attack > pcard->base_attack)
			return skin::DUELFIELD_HIGHER_CARD_ATK_VAL;
		if(pcard->attack < pcard->base_attack)
			return skin::DUELFIELD_LOWER_CARD_ATK_VAL;
		return skin::DUELFIELD_UNCHANGED_CARD_ATK_VAL;
	};

	auto GetDefColor = [&pcard] {
		if(pcard->defense > pcard->base_defense)
			return skin::DUELFIELD_HIGHER_CARD_DEF_VAL;
		if(pcard->defense < pcard->base_defense)
			return skin::DUELFIELD_LOWER_CARD_DEF_VAL;
		return skin::DUELFIELD_UNCHANGED_CARD_DEF_VAL;
	};

	auto GetLevelColor = [&pcard] {
		if(pcard->type & TYPE_TUNER)
			return skin::DUELFIELD_CARD_TUNER_LEVEL_VAL;
		return skin::DUELFIELD_CARD_LEVEL_VAL;
	};

	const auto atk = adFont->getDimensionustring(pcard->atkstring);

	const auto slash = adFont->getDimensionustring(L"/");
	const auto half_slash_width = static_cast<int>(std::floor(slash.Width / 2));

	const auto padding_1111 = Resize(1, 1, 1, 1);
	const auto padding_1011 = Resize(1, 0, 1, 1);

	if(pcard->type & TYPE_LINK) {
		DrawShadowText(adFont, pcard->atkstring, irr::core::recti(x1 - std::floor(atk.Width / 2), y1, x1 + std::floor(atk.Width / 2), y1 + 1),
					   padding_1111, GetAtkColor(), 0xff000000, true);
	} else {
		DrawShadowText(adFont, L"/", irr::core::recti(x1 - half_slash_width, y1, x1 + half_slash_width, y1 + 1), padding_1111, 0xffffffff, 0xff000000, true);
		DrawShadowText(adFont, pcard->atkstring, irr::core::recti(x1 - half_slash_width - atk.Width - slash.Width, y1, x1 - half_slash_width, y1 + 1),
					   padding_1111, GetAtkColor(), 0xff000000);
		DrawShadowText(adFont, pcard->defstring, irr::core::recti(x1 + half_slash_width + slash.Width, y1, x1 - half_slash_width, y1 + 1),
					   padding_1111, GetDefColor(), 0xff000000);
	}

	if (pcard->level != 0 && pcard->rank != 0) {
		DrawShadowText(adFont, L"/", irr::core::recti(x2 - half_slash_width, y2, x2 + half_slash_width, y2 + 1), padding_1111, 0xffffffff, 0xff000000, true);
		DrawShadowText(adFont, pcard->lvstring, irr::core::recti(x2 - half_slash_width - atk.Width - slash.Width, y2, x2 - half_slash_width, y2 + 1),
					   padding_1111, GetLevelColor(), 0xff000000);
		DrawShadowText(adFont, pcard->rkstring, irr::core::recti(x2 + half_slash_width + slash.Width, y2, x2 - half_slash_width, y2 + 1),
					   padding_1111, skin::DUELFIELD_CARD_RANK_VAL, 0xff000000);
	}
	else if (pcard->rank != 0)
		DrawShadowText(adFont, pcard->rkstring, irr::core::recti(x2, y2, x2 + 1, y2 + 1), padding_1011, skin::DUELFIELD_CARD_RANK_VAL, 0xff000000);
	else if (pcard->level != 0)
		DrawShadowText(adFont, pcard->lvstring, irr::core::recti(x2, y2, x2 + 1, y2 + 1), padding_1011, GetLevelColor(), 0xff000000);
	else if (pcard->link != 0)
		DrawShadowText(adFont, pcard->linkstring, irr::core::recti(x2, y2, x2 + 1, y2 + 1), padding_1011, skin::DUELFIELD_CARD_LINK_VAL, 0xff000000);
}
/*
Draws the pendulum scale value of a card in the pendulum zone based on its relative position
*/
void Game::DrawPendScale(ClientCard* pcard) {
	int swap = (pcard->sequence > 1 && pcard->sequence != 6) ? 1 : 0;
	float x0, y0, reverse = (pcard->controler == 0) ? 1.0f : -1.0f;
	std::wstring scale;
	if (swap) {
		x0 = pcard->curPos.X - 0.35f * reverse;
		scale = pcard->rscstring;
	} else {
		x0 = pcard->curPos.X + 0.35f * reverse;
		scale = pcard->lscstring;
	}
	if (pcard->controler == 0) {
		swap = 1 - swap;
		y0 = pcard->curPos.Y - 0.56f;
	} else
		y0 = pcard->curPos.Y + 0.29f;
	auto coords = device->getSceneManager()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition({ x0, y0, pcard->curPos.Z });
	DrawShadowText(adFont, scale, irr::core::recti(coords.X - (12 * swap), coords.Y, coords.X + (12 * (1 - swap)), coords.Y - 800),
				   Resize(1, 1, 1, 1), skin::DUELFIELD_CARD_PSCALE_VAL, 0xff000000, true);
}
/*
Draws the text in the middle of the bottom side of the zone
*/
void Game::DrawStackIndicator(epro::wstringview text, irr::video::S3DVertex* v, bool opponent) {
	const irr::core::ustring utext(text);
	const auto dim = textFont->getDimensionustring(utext) / 2;
	//int width = dim.Width / 2, height = dim.Height / 2;
	float x0 = (v[0].Pos.X + v[1].Pos.X) / 2.0f;
	float y0 = (opponent) ? v[0].Pos.Y : v[2].Pos.Y;
	auto coords = device->getSceneManager()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition({ x0, y0, 0 });
	DrawShadowText(numFont, utext, irr::core::recti(coords.X - dim.Width, coords.Y - dim.Height, coords.X + dim.Width, coords.Y + dim.Height),
				   Resize(0, 1, 0, 1), skin::DUELFIELD_STACK_VAL, 0xff000000);
}
void Game::DrawGUI() {
	if(imageLoading.size()) {
		for(auto mit = imageLoading.begin(); mit != imageLoading.end();) {
			int check = 0;
			mit->first->setImage(imageManager.GetTextureCard(mit->second, imgType::ART, false, false, &check));
			if(check != 2)
				mit = imageLoading.erase(mit);
			else
				++mit;
		}
	}
	imageManager.RefreshCachedTextures();
	for(auto fit = fadingList.begin(); fit != fadingList.end();) {
		auto fthis = fit;
		FadingUnit& fu = *fthis;
		if(fu.fadingFrame != 0.0f) {
			fit++;
			float movetime = std::min(fu.fadingFrame, (float)delta_time);
			fu.guiFading->setVisible(true);
			if(fu.isFadein) {
				if(fu.fadingFrame > (int)(5.0f * 1000.0f / 60.0f)) {
					fu.fadingUL.X -= fu.fadingDest.X * movetime;
					fu.fadingLR.X += fu.fadingDest.X * movetime;
					fu.fadingFrame -= movetime;
					if(!fu.fadingFrame)
						fu.fadingFrame += 0.0001f;
					fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				} else {
					fu.fadingUL.Y -= fu.fadingDest.Y * movetime;
					fu.fadingLR.Y += fu.fadingDest.Y * movetime;
					fu.fadingFrame -= movetime;
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
						const auto prevfocused = env->getFocus();
						env->setFocus(fu.guiFading);
						if(prevfocused && (prevfocused->getType() == irr::gui::EGUIET_EDIT_BOX))
							env->setFocus(prevfocused);
					} else
						fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				}
			} else {
				if(fu.fadingFrame > (5.0f * 1000.0f / 60.0f)) {
					fu.fadingUL.Y += fu.fadingDest.Y * movetime;
					fu.fadingLR.Y -= fu.fadingDest.Y * movetime;
					fu.fadingFrame -= movetime;
					if(!fu.fadingFrame)
						fu.fadingFrame += 0.0001f;
					fu.guiFading->setRelativePosition(irr::core::recti(fu.fadingUL, fu.fadingLR));
				} else {
					fu.fadingUL.X += fu.fadingDest.X * movetime;
					fu.fadingLR.X -= fu.fadingDest.X * movetime;
					fu.fadingFrame -= movetime;
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
			fit++;
			uint32_t movetime = std::min<uint32_t>(delta_time, fu.autoFadeoutFrame);
			fu.autoFadeoutFrame -= movetime;
			fu.guiFading->setEnabled(fu.wasEnabled);
			if(!fu.autoFadeoutFrame)
				HideElement(fu.guiFading);
		} else {
			fu.guiFading->setEnabled(fu.wasEnabled);
			fu.guiFading->setRelativePosition(fu.fadingSize);
			fit = fadingList.erase(fthis);
		}
	}
	env->drawAll();
}
void Game::DrawSpec() {
	const auto drawrect2 = ResizeWin(574, 150, 574 + CARD_IMG_WIDTH, 150 + CARD_IMG_HEIGHT);
	if(showcard) {
		switch(showcard) {
		case 1: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			driver->draw2DImage(cardtxt, drawrect2, cardrect);
			driver->draw2DImage(imageManager.tMask, ResizeWin(574, 150, 574 + (showcarddif > CARD_IMG_WIDTH ? CARD_IMG_WIDTH : showcarddif), 404),
								Scale<irr::s32>(CARD_IMG_HEIGHT - showcarddif, 0, CARD_IMG_HEIGHT - (showcarddif > CARD_IMG_WIDTH ? showcarddif - CARD_IMG_WIDTH : 0), CARD_IMG_HEIGHT), 0, 0, true);
			showcarddif += (900.0f / 1000.0f) * (float)delta_time;
			if(std::round(showcarddif) >= CARD_IMG_HEIGHT) {
				showcard = 2;
				showcarddif = 0;
			}
			break;
		}
		case 2: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			driver->draw2DImage(cardtxt, drawrect2, cardrect);
			driver->draw2DImage(imageManager.tMask, ResizeWin(574 + showcarddif, 150, 751, 404), Scale(0, 0, CARD_IMG_WIDTH - showcarddif, 254), 0, 0, true);
			showcarddif += (900.0f / 1000.0f) * (float)delta_time;
			if(showcarddif >= CARD_IMG_WIDTH) {
				showcard = 0;
			}
			break;
		}
		case 3: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			driver->draw2DImage(cardtxt, drawrect2, cardrect);
			driver->draw2DImage(imageManager.tNegated, ResizeWin(536 + showcarddif, 141 + showcarddif, 793 - showcarddif, 397 - showcarddif), Scale(0, 0, 128, 128), 0, 0, true);
			if(showcarddif < 64)
				showcarddif += (240.0f / 1000.0f) * (float)delta_time;
			break;
		}
		case 4: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			matManager.c2d[0] = ((int)std::round(showcarddif) << 24) | 0xffffff;
			matManager.c2d[1] = ((int)std::round(showcarddif) << 24) | 0xffffff;
			matManager.c2d[2] = ((int)std::round(showcarddif) << 24) | 0xffffff;
			matManager.c2d[3] = ((int)std::round(showcarddif) << 24) | 0xffffff;
			driver->draw2DImage(cardtxt, drawrect2, cardrect, 0, matManager.c2d, true);
			if(showcarddif < 255)
				showcarddif += (1020.0f / 1000.0f) * (float)delta_time;
			break;
		}
		case 5: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			matManager.c2d[0] = ((int)std::round(showcarddif) << 25) | 0xffffff;
			matManager.c2d[1] = ((int)std::round(showcarddif) << 25) | 0xffffff;
			matManager.c2d[2] = ((int)std::round(showcarddif) << 25) | 0xffffff;
			matManager.c2d[3] = ((int)std::round(showcarddif) << 25) | 0xffffff;
			auto rect = ResizeWin(662 - showcarddif * (CARD_IMG_WIDTH_F / CARD_IMG_HEIGHT_F), 277 - showcarddif, 662 + showcarddif * (CARD_IMG_WIDTH_F / CARD_IMG_HEIGHT_F), 277 + showcarddif);
			driver->draw2DImage(cardtxt, rect, cardrect, 0, matManager.c2d, true);
			if(showcarddif < 127.0f) {
				showcarddif += (540.0f / 1000.0f) * (float)delta_time;
				if(showcarddif > 127.0f)
					showcarddif = 127.0f;
			}
			break;
		}
		case 6: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			driver->draw2DImage(cardtxt, drawrect2, cardrect);
			driver->draw2DImage(imageManager.tNumber, ResizeWin(536 + showcarddif, 141 + showcarddif, 793 - showcarddif, 397 - showcarddif),
								Scale(((int)std::round(showcardp) % 5) * 64, ((int)std::round(showcardp) / 5) * 64, ((int)std::round(showcardp) % 5 + 1) * 64, ((int)std::round(showcardp) / 5 + 1) * 64), 0, 0, true);
			if(showcarddif < 64)
				showcarddif += (240.0f / 1000.0f) * (float)delta_time;
			break;
		}
		case 7: {
			auto cardtxt = imageManager.GetTextureCard(showcardcode, imgType::ART);
			auto cardrect = irr::core::rect<irr::s32>(irr::core::vector2di(0, 0), irr::core::dimension2di(cardtxt->getOriginalSize()));
			irr::core::vector2di corner[4];
			float y = sin(showcarddif * irr::core::PI / 180.0f) * CARD_IMG_HEIGHT;
			auto a = ResizeWin(574 - (CARD_IMG_HEIGHT - y) * 0.3f, (150 + CARD_IMG_HEIGHT) - y, 751 + (CARD_IMG_HEIGHT - y) * 0.3f, 150 + CARD_IMG_HEIGHT);
			auto b = ResizeWin(574, 150, 574 + CARD_IMG_WIDTH, 404);
			corner[0] = a.UpperLeftCorner;
			corner[1] = irr::core::vector2di{ a.LowerRightCorner.X, a.UpperLeftCorner.Y };
			corner[2] = irr::core::vector2di{ b.UpperLeftCorner.X, b.LowerRightCorner.Y };
			corner[3] = b.LowerRightCorner;
			irr::gui::Draw2DImageQuad(driver, cardtxt, cardrect, corner);
			showcardp += (float)delta_time * 60.0f / 1000.0f;
			showcarddif += (540.0f / 1000.0f) * (float)delta_time;
			if(showcarddif >= 90)
				showcarddif = 90;
			if(showcardp >= 60) {
				showcardp = 0;
				showcarddif = 0;
			}
			break;
		}
		case 100: {
			if(showcardp < 60) {
				driver->draw2DImage(imageManager.tHand[(showcardcode >> 16) & 0x3], Resize(615, showcarddif));
				driver->draw2DImage(imageManager.tHand[showcardcode & 0x3], Resize(615, 540 - showcarddif));
				float dy = -0.333333f * showcardp + 10;
				showcardp += (float)delta_time * 60.0f / 1000.0f;
				if(showcardp < 30)
					showcarddif += (dy * 60.f / 1000.0f) * (float)delta_time;
			} else
				showcard = 0;
			break;
		}
		case 101: {
			irr::core::ustring lstr = L"";
			if(1 <= showcardcode && showcardcode <= 14) {
				lstr = gDataManager->GetSysString(1700 + showcardcode);
			}
			auto pos = lpcFont->getDimensionustring(lstr);
			if(showcardp < 10.0f) {
				int alpha = ((int)std::round(showcardp) * 25) << 24;
				DrawShadowText(lpcFont, lstr, ResizePhaseHint(661 - (9 - showcardp) * 40, 291, 960, 370, pos.Width), Resize(-1, -1, 0, 0), alpha | 0xffffff, alpha);
			} else if(showcardp < showcarddif) {
				DrawShadowText(lpcFont, lstr, ResizePhaseHint(661, 291, 960, 370, pos.Width), Resize(-1, -1, 0, 0), 0xffffffff);
				if(dInfo.vic_string.size() && (showcardcode == 1 || showcardcode == 2)) {
					auto a = (291 + pos.Height + 2);
					driver->draw2DRectangle(0xa0000000, Resize(540, a, 790, a + 20));
					DrawShadowText(guiFont, dInfo.vic_string, Resize(492, a + 1, 840, a + 20), Resize(-2, -1, 0, 0), 0xffffffff, 0xff000000, true, true);
				}
			} else if(showcardp < showcarddif + 10.0f) {
				int alpha = (int)std::round((((showcarddif + 10.0f - showcardp) * 25.0f) / 1000.0f) * (float)delta_time) << 24;
				DrawShadowText(lpcFont, lstr, ResizePhaseHint(661 + (showcardp - showcarddif) * 40, 291, 960, 370, pos.Width), Resize(-1, -1, 0, 0), alpha | 0xffffff, alpha);
			}
			showcardp += std::min(((float)delta_time * 60.0f / 1000.0f), showcarddif - showcardp);
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
		driver->drawVertexPrimitiveList(&matManager.vArrow[std::min(static_cast<int>(attack_sv) * 4, 28)], 12, matManager.iArrow, 10, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLE_STRIP);
		attack_sv += (60.0f / 1000.0f) * delta_time;
		if (static_cast<int>(attack_sv) > 9)
			attack_sv = 0.0f;
	}
	bool showChat = true;
	if(hideChat) {
		showChat = false;
		hideChatTimer = 10;
	} else if(hideChatTimer > 0) {
		showChat = false;
		hideChatTimer--;
	}
	for(int i = 0; i < 8; ++i) {
		static constexpr uint32_t chatColor[] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff8080ff, 0xffff4040, 0xffff4040,
										   0xffff4040, 0xff40ff40, 0xff4040ff, 0xff40ffff, 0xffff40ff, 0xffffff40, 0xffffffff, 0xff808080, 0xff404040};
		if(chatTiming[i] > 0.0f) {
			chatTiming[i] -= (float)delta_time * 60.0f / 1000.0f;
			if(dInfo.isStarted && i >= 5)
				continue;
			if(!showChat && i > 2)
				continue;
			int w = textFont->getDimensionustring(chatMsg[i]).Width;
			irr::core::recti chatrect = wChat->getRelativePosition();
			auto rectloc = chatrect;
			rectloc -= irr::core::vector2di(0, (i + 1) * chatrect.getHeight() + Scale(1));
			rectloc.LowerRightCorner.X = rectloc.UpperLeftCorner.X + w + Scale(4);
			auto msgloc = chatrect;
			msgloc -= irr::core::vector2di(Scale(-2), (i + 1) * chatrect.getHeight() + Scale(1));
			auto shadowloc = msgloc + irr::core::vector2di(1, 1);
			driver->draw2DRectangle(rectloc, 0xa0000000, 0xa0000000, 0xa0000000, 0xa0000000);
			textFont->drawustring(chatMsg[i], msgloc, 0xff000000, false, false);
			textFont->drawustring(chatMsg[i], shadowloc, chatColor[chatType[i]], false, false);
		}
	}
}
void Game::DrawBackImage(irr::video::ITexture* texture, bool resized) {
	static irr::video::ITexture* prevbg = nullptr;
	static irr::core::recti dest_size = { 0,0,0,0 };
	static irr::core::recti bg_size = { 0,0,0,0 };
	static bool was_scaled = false;
	if(was_scaled && !gGameConfig->scale_background) {
		was_scaled = false;
		prevbg = nullptr;
	} else if(!was_scaled && gGameConfig->scale_background) {
		was_scaled = true;
		prevbg = nullptr;
	}
	if(resized)
		prevbg = nullptr;
	if(!texture)
		return;
	if(texture != prevbg) {
		prevbg = texture;
		dest_size = Resize(0, 0, 1024, 640);
		bg_size = irr::core::recti(0, 0, texture->getOriginalSize().Width, texture->getOriginalSize().Height);
		if(!gGameConfig->scale_background) {
			irr::core::rectf dest_sizef(0, 0, dest_size.getWidth(), dest_size.getHeight());
			irr::core::rectf bg_sizef(0, 0, bg_size.getWidth(), bg_size.getHeight());
			float width = ((bg_sizef.getWidth() / bg_sizef.getHeight()) * dest_sizef.getHeight()) - dest_sizef.getWidth();
			float height = ((bg_sizef.getHeight() / bg_sizef.getWidth()) * dest_sizef.getWidth()) - dest_sizef.getHeight();
			if(width > 0) {
				int off = std::ceil(width * 0.5f);
				dest_size = irr::core::recti({ -off, 0, dest_size.getWidth() + off, dest_size.getHeight() });
			} else if(height > 0) {
				int off = std::ceil(height * 0.5f);
				dest_size = irr::core::recti({ 0, -off, dest_size.getWidth(), dest_size.getHeight() + off });
			}
		}
	}
	if(gGameConfig->accurate_bg_resize)
		imageManager.draw2DImageFilterScaled(texture, dest_size, bg_size);
	else
		driver->draw2DImage(texture, dest_size, bg_size);
}
void Game::ShowElement(irr::gui::IGUIElement * win, int autoframe) {
	FadingUnit fu;
	fu.fadingSize = win->getRelativePosition();
	fu.wasEnabled = win->isEnabled();
	win->setEnabled(false);
	for(auto fit = fadingList.begin(); fit != fadingList.end(); ++fit) {
		if(win == fit->guiFading) {
			if(win != wOptions) // the size of wOptions is always set by ClientField::ShowSelectOption before showing it
				fu.fadingSize = fit->fadingSize;
			fu.wasEnabled = fit->wasEnabled;
		}
	}
	irr::core::vector2di center = fu.fadingSize.getCenter();
	fu.fadingFrame = 10.0f * 1000.0f / 60.0f;
	fu.fadingDest.X = fu.fadingSize.getWidth() / fu.fadingFrame;
	fu.fadingDest.Y = (fu.fadingSize.getHeight() - 4) / fu.fadingFrame;
	fu.fadingUL = center;
	fu.fadingLR = center;
	fu.fadingUL.Y -= 2;
	fu.fadingLR.Y += 2;
	fu.guiFading = win;
	fu.isFadein = true;
	fu.autoFadeoutFrame = autoframe * 1000 / 60;
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
	win->setRelativePosition(Scale(center.X, center.Y, 0, 0));
	fadingList.push_back(fu);
}
void Game::HideElement(irr::gui::IGUIElement * win, bool set_action) {
	FadingUnit fu;
	fu.fadingSize = win->getRelativePosition();
	fu.wasEnabled = win->isEnabled();
	win->setEnabled(false);
	for(auto fit = fadingList.begin(); fit != fadingList.end(); ++fit) {
		if(win == fit->guiFading) {
			fu.fadingSize = fit->fadingSize;
			fu.wasEnabled = fit->wasEnabled;
		}
	}
	fu.fadingFrame = 10.0f * 1000.0f / 60.0f;
	fu.fadingDest.X = fu.fadingSize.getWidth() / fu.fadingFrame;
	fu.fadingDest.Y = (fu.fadingSize.getHeight() - 4) / fu.fadingFrame;
	fu.fadingUL = fu.fadingSize.UpperLeftCorner;
	fu.fadingLR = fu.fadingSize.LowerRightCorner;
	fu.guiFading = win;
	fu.isFadein = false;
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
	if(!is_building)
		dField.panel = element;
	const auto prevfocused = env->getFocus();
	env->setFocus(element);
	if(prevfocused && (prevfocused->getType() == irr::gui::EGUIET_EDIT_BOX))
		env->setFocus(prevfocused);
	if(!hideframe)
		ShowElement(element);
	else ShowElement(element, hideframe);
}
void Game::WaitFrameSignal(int frame, std::unique_lock<std::mutex>& _lck) {
	signalFrame = (gGameConfig->quick_animation && frame >= 12) ? 12 * 1000 / 60 : frame * 1000 / 60;
	frameSignal.Wait(_lck);
}
void Game::DrawThumb(const CardDataC* cp, irr::core::position2di pos, LFList* lflist, bool drag, const irr::core::recti* cliprect, bool load_image) {
	auto code = cp->code;
	auto flit = lflist->GetLimitationIterator(cp);
	int count = 3;
	if(flit == lflist->content.end()) {
		if(lflist->whitelist)
			count = -1;
	} else
		count = flit->second;
	irr::video::ITexture* img = load_image ? imageManager.GetTextureCard(code, imgType::THUMB) : imageManager.tUnknown;
	if (!img)
		return;
	irr::core::dimension2du size = img->getOriginalSize();
	irr::core::recti dragloc = Resize(pos.X, pos.Y, pos.X + CARD_THUMB_WIDTH, pos.Y + CARD_THUMB_HEIGHT);
	irr::core::recti limitloc = Resize(pos.X, pos.Y, pos.X + 20, pos.Y + 20);
	irr::core::recti otloc = Resize(pos.X + 7, pos.Y + 50, pos.X + 37, pos.Y + 65);
	if(drag) {
		dragloc = irr::core::recti(pos.X, pos.Y, pos.X + Scale(CARD_THUMB_WIDTH * window_scale.X), pos.Y + Scale(CARD_THUMB_HEIGHT * window_scale.Y));
		limitloc = irr::core::recti(pos.X, pos.Y, pos.X + Scale(20 * window_scale.X), pos.Y + Scale(20 * window_scale.Y));
		otloc = irr::core::recti(pos.X + 7, pos.Y + 50 * window_scale.Y, pos.X + 37 * window_scale.X, pos.Y + 65 * window_scale.Y);
	}
	driver->draw2DImage(img, dragloc, irr::core::recti(0, 0, size.Width, size.Height), cliprect);
	if(!is_siding) {
		switch(count) {
			case -1:
			case 0:
				imageManager.draw2DImageFilterScaled(imageManager.tLim, limitloc, irr::core::recti(0, 0, 64, 64), cliprect, 0, true);
				break;
			case 1:
				imageManager.draw2DImageFilterScaled(imageManager.tLim, limitloc, irr::core::recti(64, 0, 128, 64), cliprect, 0, true);
				break;
			case 2:
				imageManager.draw2DImageFilterScaled(imageManager.tLim, limitloc, irr::core::recti(0, 64, 64, 128), cliprect, 0, true);
				break;
		}
#define IDX(scope,idx) case SCOPE_##scope:\
							index = idx;\
							goto draw;
		if(gGameConfig->showScopeLabel && !lflist->whitelist) {
			// Label display logic:
			// If it contains exactly one bit between Anime, Illegal, Video Game, Custom, and Prerelease, display that.
			// Else, if it contains exactly one bit between OCG and TCG, display that.
			switch(cp->ot & ~(SCOPE_PRERELEASE | SCOPE_LEGEND)) {
				int index;
				IDX(OCG,0)
				IDX(TCG,1)
				IDX(ILLEGAL,2)
				IDX(ANIME,3)
				IDX(VIDEO_GAME,5)
				IDX(CUSTOM,6)
				IDX(SPEED,8)
				IDX(RUSH,9)
				default: break;
				draw:
				imageManager.draw2DImageFilterScaled(imageManager.tOT, otloc, irr::core::recti(0, index * 64, 128, index * 64 + 64), cliprect, 0, true);
			}
		}
#undef IDX
	}
}
#define SKCOLOR(what) skin::DECK_WINDOW_##what##_VAL
#define DECKCOLOR(what) SKCOLOR(what##_TOP_LEFT), SKCOLOR(what##_TOP_RIGHT), SKCOLOR(what##_BOTTOM_LEFT), SKCOLOR(what##_BOTTOM_RIGHT)
#define DRAWRECT(what,...) do { driver->draw2DRectangle(Resize(__VA_ARGS__), DECKCOLOR(what)); } while(0)
#define DRAWOUTLINE(what,...) do { driver->draw2DRectangleOutline(Resize(__VA_ARGS__), SKCOLOR(what##_OUTLINE)); } while(0)
void Game::DrawDeckBd() {
	const auto GetDeckSizeStr = [&](const Deck::Vector& deck, const Deck::Vector& pre_deck)->std::wstring {
		if(is_siding)
			return fmt::format(L"{} ({})", deck.size(), pre_deck.size());
		return fmt::to_wstring(deck.size());
	};
	const auto& current_deck = deckBuilder.GetCurrentDeck();

	//main deck
	{
		DRAWRECT(MAIN_INFO, 310, 137, 797, 157);
		DRAWOUTLINE(MAIN_INFO, 309, 136, 797, 157);

		DrawShadowText(textFont, gDataManager->GetSysString(1330), Resize(314, 136, 409, 156), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto main_deck_size_str = GetDeckSizeStr(current_deck.main, gdeckManager->pre_deck.main);
		DrawShadowText(numFont, main_deck_size_str, Resize(379, 137, 439, 157), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto main_types_count_str = fmt::format(L"{} {} {} {} {} {}",
													  gDataManager->GetSysString(1312), deckBuilder.main_monster_count,
													  gDataManager->GetSysString(1313), deckBuilder.main_spell_count,
													  gDataManager->GetSysString(1314), deckBuilder.main_trap_count);

		const auto mainpos = Resize(310, 137, 797, 157);
		const auto mainDeckTypeSize = textFont->getDimensionustring(main_types_count_str);
		const auto pos = irr::core::recti(mainpos.LowerRightCorner.X - mainDeckTypeSize.Width - 5, mainpos.UpperLeftCorner.Y,
										  mainpos.LowerRightCorner.X, mainpos.LowerRightCorner.Y);

		DrawShadowText(textFont, main_types_count_str, pos, irr::core::recti{ 1, 1, 1, 1 }, 0xffffffff, 0xff000000, false, true);

		DRAWRECT(MAIN, 310, 160, 797, 436);
		DRAWOUTLINE(MAIN, 309, 159, 797, 436);

		const int lx = (current_deck.main.size() > 40) ? ((current_deck.main.size() - 41) / 4 + 11) : 10;
		const float dx = 436.0f / (lx - 1);

		for(size_t i = 0; i < current_deck.main.size(); ++i) {
			DrawThumb(current_deck.main[i], irr::core::vector2di(314 + (i % lx) * dx, 164 + (i / lx) * 68), deckBuilder.filterList);
			if(deckBuilder.hovered_pos == 1 && deckBuilder.hovered_seq == (int)i)
				driver->draw2DRectangleOutline(Resize(313 + (i % lx) * dx, 163 + (i / lx) * 68, 359 + (i % lx) * dx, 228 + (i / lx) * 68), skin::DECK_WINDOW_HOVERED_CARD_OUTLINE_VAL);
		}
	}
	//extra deck
	{
		DRAWRECT(EXTRA_INFO, 310, 440, 797, 460);
		DRAWOUTLINE(EXTRA_INFO, 309, 439, 797, 460);

		DrawShadowText(textFont, gDataManager->GetSysString(1331), Resize(314, 439, 409, 459), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto extra_deck_size_str = GetDeckSizeStr(current_deck.extra, gdeckManager->pre_deck.extra);
		DrawShadowText(numFont, extra_deck_size_str, Resize(379, 440, 439, 460), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto extra_types_count_str = fmt::format(L"{} {} {} {} {} {} {} {}",
													   gDataManager->GetSysString(1056), deckBuilder.extra_fusion_count,
													   gDataManager->GetSysString(1073), deckBuilder.extra_xyz_count,
													   gDataManager->GetSysString(1063), deckBuilder.extra_synchro_count,
													   gDataManager->GetSysString(1076), deckBuilder.extra_link_count);

		const auto extrapos = Resize(310, 440, 797, 460);
		const auto extraDeckTypeSize = textFont->getDimensionustring(extra_types_count_str);
		const auto pos = irr::core::recti(extrapos.LowerRightCorner.X - extraDeckTypeSize.Width - 5, extrapos.UpperLeftCorner.Y,
										  extrapos.LowerRightCorner.X, extrapos.LowerRightCorner.Y);

		DrawShadowText(textFont, extra_types_count_str, pos, irr::core::recti{ 1, 1, 1, 1 }, 0xffffffff, 0xff000000, false, true);

		DRAWRECT(EXTRA, 310, 463, 797, 533);
		DRAWOUTLINE(EXTRA, 309, 462, 797, 533);

		const float dx = (current_deck.extra.size() <= 10) ? (436.0f / 9.0f) : (436.0f / (current_deck.extra.size() - 1));

		for(size_t i = 0; i < current_deck.extra.size(); ++i) {
			DrawThumb(current_deck.extra[i], irr::core::vector2di(314 + i * dx, 466), deckBuilder.filterList);
			if(deckBuilder.hovered_pos == 2 && deckBuilder.hovered_seq == (int)i)
				driver->draw2DRectangleOutline(Resize(313 + i * dx, 465, 359 + i * dx, 531), skin::DECK_WINDOW_HOVERED_CARD_OUTLINE_VAL);
		}
	}
	//side deck
	{
		DRAWRECT(SIDE_INFO, 310, 537, 797, 557);
		DRAWOUTLINE(SIDE_INFO, 309, 536, 797, 557);

		DrawShadowText(textFont, gDataManager->GetSysString(1332), Resize(314, 536, 409, 556), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto side_deck_size_str = GetDeckSizeStr(current_deck.side, gdeckManager->pre_deck.side);
		DrawShadowText(numFont, side_deck_size_str, Resize(379, 536, 439, 556), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto side_types_count_str = fmt::format(L"{} {} {} {} {} {}",
													  gDataManager->GetSysString(1312), deckBuilder.side_monster_count,
													  gDataManager->GetSysString(1313), deckBuilder.side_spell_count,
													  gDataManager->GetSysString(1314), deckBuilder.side_trap_count);

		const auto sidepos = Resize(310, 537, 797, 557);
		const auto sideDeckTypeSize = textFont->getDimensionustring(side_types_count_str);
		const auto pos = irr::core::recti(sidepos.LowerRightCorner.X - sideDeckTypeSize.Width - 5, sidepos.UpperLeftCorner.Y,
										  sidepos.LowerRightCorner.X, sidepos.LowerRightCorner.Y);

		DrawShadowText(textFont, side_types_count_str, pos, irr::core::recti{ 1, 1, 1, 1 }, 0xffffffff, 0xff000000, false, true);
		DRAWRECT(SIDE, 310, 560, 797, 630);
		DRAWOUTLINE(SIDE, 309, 559, 797, 630);

		const float dx = (current_deck.side.size() <= 10) ? (436.0f / 9.0f) : (436.0f / (current_deck.side.size() - 1));

		for(size_t i = 0; i < current_deck.side.size(); ++i) {
			DrawThumb(current_deck.side[i], irr::core::vector2di(314 + i * dx, 564), deckBuilder.filterList);
			if(deckBuilder.hovered_pos == 3 && deckBuilder.hovered_seq == (int)i)
				driver->draw2DRectangleOutline(Resize(313 + i * dx, 563, 359 + i * dx, 629), skin::DECK_WINDOW_HOVERED_CARD_OUTLINE_VAL);
		}
	}
	//search result
	{
		DRAWRECT(SEARCH_RESULT_INFO, 805, 137, 915, 157);
		DRAWOUTLINE(SEARCH_RESULT_INFO, 804, 136, 915, 157);

		DrawShadowText(textFont, gDataManager->GetSysString(1333), Resize(809, 136, 914, 156), Resize(1, 1, 1, 1), 0xffffffff, 0xff000000, false, true);

		const auto tmpstring = gDataManager->GetSysString(1333);
		const auto size = textFont->getDimensionustring(tmpstring).Width + ResizeX(5);
		const auto pos = irr::core::recti(ResizeX(809) + size, ResizeY(136), ResizeX(809) + size + 10, ResizeY(156));
		DrawShadowText(numFont, deckBuilder.result_string, pos, Resize(0, 1, 0, 1), 0xffffffff, 0xff000000, false, true);

		DRAWRECT(SEARCH_RESULT, 805, 160, 1020, 630);
		DRAWOUTLINE(SEARCH_RESULT, 804, 159, 1020, 630);

		const int prev_pos = deckBuilder.scroll_pos;
		deckBuilder.scroll_pos = floor(scrFilter->getPos() / DECK_SEARCH_SCROLL_STEP);

		const bool draw_thumb = std::abs(prev_pos - deckBuilder.scroll_pos) < (10.0f * 60.0f / 1000.0f) * delta_time;
		const int card_position = deckBuilder.scroll_pos;
		const int height_offset = (scrFilter->getPos() % DECK_SEARCH_SCROLL_STEP) * -1.f * 0.65f;
		const irr::core::recti rect = Resize(805, 160, 1020, 630);

		//loads the thumb of one card before and one after to make the scroll smoother
		int i = (card_position > 0) ? -1 : 0;
		for(; i < 9 && (i + card_position) < (int)deckBuilder.results.size(); ++i) {
			auto ptr = deckBuilder.results[i + card_position];
			if(deckBuilder.hovered_pos == 4 && deckBuilder.hovered_seq == i)
				driver->draw2DRectangle(skin::DECK_WINDOW_HOVERED_CARD_RESULT_VAL, Resize(806, height_offset + 164 + i * 66, 1019, height_offset + 230 + i * 66), &rect);
			DrawThumb(ptr, irr::core::vector2di(810, height_offset + 165 + i * 66), deckBuilder.filterList, false, &rect, draw_thumb);
			if(ptr->type & TYPE_MONSTER) {
				DrawShadowTextPos(textFont, gDataManager->GetName(ptr->code), Resize(859, height_offset + 164 + i * 66, 955, height_offset + 185 + i * 66),
								  Resize(860, height_offset + 165 + i * 66, 955, height_offset + 185 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
				if(ptr->type & TYPE_LINK) {
					DrawShadowTextPos(textFont, fmt::format(L"{}/{}", gDataManager->FormatAttribute(ptr->attribute), gDataManager->FormatRace(ptr->race)),
									  Resize(859, height_offset + 186 + i * 66, 955, height_offset + 207 + i * 66),
									  Resize(860, height_offset + 187 + i * 66, 955, height_offset + 207 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
				} else {
					const wchar_t* form = L"\u2605";
					if(ptr->type & TYPE_XYZ) form = L"\u2606";
					DrawShadowTextPos(textFont, fmt::format(L"{}/{} {}{}", gDataManager->FormatAttribute(ptr->attribute), gDataManager->FormatRace(ptr->race), form, ptr->level),
									  Resize(859, height_offset + 186 + i * 66, 955, height_offset + 207 + i * 66),
									  Resize(860, height_offset + 187 + i * 66, 955, height_offset + 207 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
				}
			} else {
				DrawShadowTextPos(textFont, gDataManager->GetName(ptr->code), Resize(859, height_offset + 164 + i * 66, 955, height_offset + 185 + i * 66),
								  Resize(860, height_offset + 165 + i * 66, 955, height_offset + 185 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
				DrawShadowTextPos(textFont, gDataManager->FormatType(ptr->type), Resize(859, height_offset + 186 + i * 66, 955, height_offset + 207 + i * 66),
								  Resize(860, height_offset + 187 + i * 66, 955, height_offset + 207 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
			}
			auto GetScopeString = [&ptr]()->std::wstring {
				auto scope = gDataManager->FormatScope(ptr->ot, true);
				if(ptr->type & TYPE_MONSTER) {
					std::wstring buffer;
					if(ptr->type & TYPE_LINK) {
						if(ptr->attack < 0)
							buffer = fmt::format(L"?/Link {}\t", ptr->level);
						else
							buffer = fmt::format(L"{}/Link {}\t", ptr->attack, ptr->level);
					} else {
						if(ptr->attack < 0 && ptr->defense < 0)
							buffer = L"?/?";
						else if(ptr->attack < 0)
							buffer = fmt::format(L"?/{}", ptr->defense);
						else if(ptr->defense < 0)
							buffer = fmt::format(L"{}/?", ptr->attack);
						else
							buffer = fmt::format(L"{}/{}", ptr->attack, ptr->defense);
					}
					if(ptr->type & TYPE_PENDULUM)
						buffer.append(fmt::format(L" {}/{}", ptr->lscale, ptr->rscale));
					if(!scope.empty())
						return fmt::format(L"{} [{}]", buffer, scope);
					return buffer;
				}
				if(!scope.empty())
					return fmt::format(L"[{}]", scope);
				return L"";
			};
			DrawShadowTextPos(textFont, GetScopeString(), Resize(859, height_offset + 208 + i * 66, 955, height_offset + 229 + i * 66),
							  Resize(860, height_offset + 209 + i * 66, 955, height_offset + 229 + i * 66), 0xffffffff, 0xff000000, false, false, &rect);
		}
	}
	if(deckBuilder.is_draging)
		DrawThumb(deckBuilder.dragging_pointer, irr::core::vector2di(deckBuilder.dragx - Scale(CARD_THUMB_WIDTH / 2), deckBuilder.dragy - Scale(CARD_THUMB_HEIGHT / 2)), deckBuilder.filterList, true);
}
#undef DRAWRECT
#undef DECKCOLOR
#undef SKCOLOR
}
