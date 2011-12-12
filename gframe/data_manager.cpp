#include "data_manager.h"
#include <stdio.h>

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
const wchar_t* DataManager::effect_strings[] = {
	L"魔陷破坏", L"怪兽破坏", L"卡片除外", L"送去墓地", L"返回手牌", L"返回卡组", L"手牌破坏", L"卡组破坏",
	L"抽卡辅助", L"卡组检索", L"卡片回收", L"表示形式", L"控制权  ", L"攻守变化", L"穿刺伤害", L"多次攻击",
	L"攻击限制", L"直接攻击", L"特殊召唤", L"衍生物  ", L"种族相关", L"属性相关", L"LP伤害  ", L"LP回复  ",
	L"破坏耐性", L"效果耐性", L"指示物  ", L"幸运    ", L"融合相关", L"同调相关", L"超量相关", L"效果无效"
};
wchar_t DataManager::strBuffer[2048];
wchar_t DataManager::attBuffer[128];
wchar_t DataManager::racBuffer[128];
wchar_t DataManager::tpBuffer[128];

bool DataManager::LoadDates(const char* file) {
	sqlite3* pDB;
	if(sqlite3_open(file, &pDB) != SQLITE_OK)
		return Error(pDB);
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	CardDataC cd;
	CardString cs;
	for(int i = 0; i < 16; ++i) cs.desc[i] = 0;
	int step = 0, len = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if(step == SQLITE_ROW) {
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.alias = sqlite3_column_int(pStmt, 2);
			cd.setcode = sqlite3_column_int(pStmt, 3);
			cd.type = sqlite3_column_int(pStmt, 4);
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defence = sqlite3_column_int(pStmt, 6);
			cd.level = sqlite3_column_int(pStmt, 7);
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			_datas.insert(std::make_pair(cd.code, cd));
			len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), strBuffer);
			if(len) {
				cs.name = new wchar_t[len + 1];
				memcpy(cs.name, strBuffer, (len + 1)*sizeof(wchar_t));
			} else cs.name = 0;
			len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), strBuffer);
			if(len) {
				cs.text = new wchar_t[len + 1];
				memcpy(cs.text, strBuffer, (len + 1)*sizeof(wchar_t));
			} else {
				cs.text = new wchar_t[1];
				cs.text[0] = 0;
			}
			for(int i = 14; i < 30; ++i) {
				len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, i), strBuffer);
				if(len) {
					cs.desc[i - 14] = new wchar_t[len + 1];
					memcpy(cs.desc[i - 14], strBuffer, (len + 1)*sizeof(wchar_t));
				} else break;
			}
			_strings.insert(std::make_pair(cd.code, cs));
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	FILE* fp = fopen("strings.conf", "r");
	if(!fp)
		return false;
	char linebuf[256];
	char strbuf[256];
	int value;
	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fgets(linebuf, 256, fp);
	while(ftell(fp) < fsize) {
		fgets(linebuf, 256, fp);
		if(linebuf[0] != '!')
			continue;
		sscanf(linebuf, "!%s", strbuf);
		if(!strcmp(strbuf, "system")) {
			sscanf(&linebuf[7], "%d %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_sysStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "victory")) {
			sscanf(&linebuf[8], "%x %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_victoryStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "counter")) {
			sscanf(&linebuf[8], "%x %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_counterStrings[value] = pbuf;
		}
	}
	fclose(fp);
	for(int i = 0; i < 255; ++i)
		myswprintf(numStrings[i], L"%d", i);
	return true;
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	DecodeUTF8(sqlite3_errmsg(pDB), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return false;
}
bool DataManager::GetData(int code, CardData* pData) {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if(pData)
		*pData = *((CardData*)&cdit->second);
	return true;
}
code_pointer DataManager::GetCodePointer(int code) {
	return _datas.find(code);
}
bool DataManager::GetString(int code, CardString* pStr) {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = (wchar_t*)unknown_string;
		pStr->text = (wchar_t*)unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
const wchar_t* DataManager::GetName(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.name)
		return csit->second.name;
	return unknown_string;
}
const wchar_t* DataManager::GetText(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.text)
		return csit->second.text;
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(int strCode) {
	if(strCode < 10000)
		return GetSysString(strCode);
	int code = strCode >> 4;
	int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.desc[offset])
		return csit->second.desc[offset];
	return unknown_string;
}
const wchar_t* DataManager::GetSysString(int code) {
	auto csit = _sysStrings.find(code);
	if(csit == _sysStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetNumString(int num) {
	return numStrings[num];
}
int DataManager::EncodeUTF8(const wchar_t * wsrc, char * str) {
	char* pstr = str;
	while(*wsrc != 0) {
		if(*wsrc < 0x80) {
			*str = *wsrc;
			++str;
		} else if(*wsrc < 0x800) {
			str[0] = (*wsrc >> 6) & 0x1f | 0xc0;
			str[1] = (*wsrc) & 0x3f | 0x80;
			str += 2;
		} else {
			str[0] = (*wsrc >> 12) & 0xf | 0xe0;
			str[1] = (*wsrc >> 6) & 0x3f | 0x80;
			str[2] = (*wsrc) & 0x3f | 0x80;
			str += 3;
		}
		wsrc++;
	}
	*str = 0;
	return str - pstr;
}
int DataManager::DecodeUTF8(const char * src, wchar_t * wstr) {
	char* p = (char*)src;
	wchar_t* wp = wstr;
	while(*p != 0) {
		if((*p & 0x80) == 0) {
			*wp = *p;
			p++;
		} else if((*p & 0xe0) == 0xc0) {
			*wp = (((int)p[0] & 0x1f) << 6) | ((int)p[1] & 0x3f);
			p += 2;
		} else if((*p & 0xf0) == 0xe0) {
			*wp = (((int)p[0] & 0xf) << 12) | (((int)p[1] & 0x3f) << 6) | ((int)p[2] & 0x3f);
			p += 3;
		} else if((*p & 0xf8) == 0xf0) {
			*wp = (((int)p[0] & 0x7) << 18) | (((int)p[1] & 0x3f) << 12) | (((int)p[2] & 0x3f) << 6) | ((int)p[3] & 0x3f);
			p += 4;
		} else
			p++;
		wp++;
	}
	*wp = 0;
	return wp - wstr;
}
int DataManager::GetVal(const wchar_t* pstr) {
	int ret = 0;
	while(*pstr >= L'0' && *pstr <= L'9') {
		ret = ret * 10 + (*pstr - L'0');
		pstr++;
	}
	return ret;
}
const wchar_t* DataManager::FormatLocation(int location) {
	switch(location) {
	case LOCATION_DECK:
		return L"卡组";
	case LOCATION_HAND:
		return L"手牌";
	case LOCATION_MZONE:
		return L"怪兽区";
	case LOCATION_SZONE:
		return L"魔法陷阱区";
	case LOCATION_GRAVE:
		return L"墓地";
	case LOCATION_REMOVED:
		return L"除外";
	case LOCATION_EXTRA:
		return L"额外";
	case LOCATION_OVERLAY:
		return L"叠放";
	}
	return unknown_string;
}
const wchar_t* DataManager::FormatAttribute(int attribute) {
	wchar_t* p = attBuffer;
	if(attribute & ATTRIBUTE_EARTH) {
		wcscpy(p, L"地|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_WATER) {
		wcscpy(p, L"水|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_FIRE) {
		wcscpy(p, L"炎|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_WIND) {
		wcscpy(p, L"风|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_LIGHT) {
		wcscpy(p, L"光|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_DARK) {
		wcscpy(p, L"暗|");
		p += 2;
	}
	if(attribute & ATTRIBUTE_DEVINE) {
		wcscpy(p, L"神|");
		p += 2;
	}
	if(p != attBuffer)
		*(p - 1) = 0;
	else *p = 0;
	return attBuffer;
}
const wchar_t* DataManager::FormatRace(int race) {
	wchar_t* p = racBuffer;
	if(race & RACE_WARRIOR) {
		wcscpy(p, L"战士族|");
		p += 4;
	}
	if(race & RACE_SPELLCASTER) {
		wcscpy(p, L"魔法使族|");
		p += 5;
	}
	if(race & RACE_FAIRY) {
		wcscpy(p, L"天使族|");
		p += 4;
	}
	if(race & RACE_FIEND) {
		wcscpy(p, L"恶魔族|");
		p += 4;
	}
	if(race & RACE_ZOMBIE) {
		wcscpy(p, L"不死族|");
		p += 4;
	}
	if(race & RACE_MACHINE) {
		wcscpy(p, L"机械族|");
		p += 4;
	}
	if(race & RACE_AQUA) {
		wcscpy(p, L"水族|");
		p += 3;
	}
	if(race & RACE_PYRO) {
		wcscpy(p, L"炎族|");
		p += 3;
	}
	if(race & RACE_ROCK) {
		wcscpy(p, L"岩石族|");
		p += 4;
	}
	if(race & RACE_WINDBEAST) {
		wcscpy(p, L"鸟兽族|");
		p += 4;
	}
	if(race & RACE_PLANT) {
		wcscpy(p, L"植物族|");
		p += 4;
	}
	if(race & RACE_INSECT) {
		wcscpy(p, L"昆虫族|");
		p += 4;
	}
	if(race & RACE_THUNDER) {
		wcscpy(p, L"雷族|");
		p += 3;
	}
	if(race & RACE_DRAGON) {
		wcscpy(p, L"龙族|");
		p += 3;
	}
	if(race & RACE_BEAST) {
		wcscpy(p, L"兽族|");
		p += 3;
	}
	if(race & RACE_BEASTWARRIOR) {
		wcscpy(p, L"兽战士族|");
		p += 5;
	}
	if(race & RACE_DINOSAUR) {
		wcscpy(p, L"恐龙族|");
		p += 4;
	}
	if(race & RACE_FISH) {
		wcscpy(p, L"鱼族|");
		p += 3;
	}
	if(race & RACE_SEASERPENT) {
		wcscpy(p, L"海龙族|");
		p += 4;
	}
	if(race & RACE_REPTILE) {
		wcscpy(p, L"爬虫族|");
		p += 4;
	}
	if(race & RACE_PSYCHO) {
		wcscpy(p, L"念动力族|");
		p += 5;
	}
	if(race & RACE_DEVINE) {
		wcscpy(p, L"幻神兽族|");
		p += 5;
	}
	if(p != racBuffer)
		*(p - 1) = 0;
	else *p = 0;
	return racBuffer;
}
const wchar_t* DataManager::FormatType(int type) {
	wchar_t* p = tpBuffer;
	if(type & TYPE_MONSTER) {
		wcscpy(p, L"怪兽|");
		p += 3;
	}
	if(type & TYPE_SPELL) {
		wcscpy(p, L"魔法|");
		p += 3;
	}
	if(type & TYPE_TRAP) {
		wcscpy(p, L"陷阱|");
		p += 3;
	}
	if(type & TYPE_NORMAL) {
		wcscpy(p, L"通常|");
		p += 3;
	}
	if(type & TYPE_EFFECT) {
		wcscpy(p, L"效果|");
		p += 3;
	}
	if(type & TYPE_FUSION) {
		wcscpy(p, L"融合|");
		p += 3;
	}
	if(type & TYPE_RITUAL) {
		wcscpy(p, L"仪式|");
		p += 3;
	}
	if(type & TYPE_SPIRIT) {
		wcscpy(p, L"灵魂|");
		p += 3;
	}
	if(type & TYPE_UNION) {
		wcscpy(p, L"同盟|");
		p += 3;
	}
	if(type & TYPE_DUAL) {
		wcscpy(p, L"二重|");
		p += 3;
	}
	if(type & TYPE_TUNER) {
		wcscpy(p, L"调整|");
		p += 3;
	}
	if(type & TYPE_SYNCHRO) {
		wcscpy(p, L"同调|");
		p += 3;
	}
	if(type & TYPE_TOKEN) {
		wcscpy(p, L"衍生物|");
		p += 4;
	}
	if(type & TYPE_QUICKPLAY) {
		wcscpy(p, L"速攻|");
		p += 3;
	}
	if(type & TYPE_CONTINUOUS) {
		wcscpy(p, L"永续|");
		p += 3;
	}
	if(type & TYPE_EQUIP) {
		wcscpy(p, L"装备|");
		p += 3;
	}
	if(type & TYPE_FIELD) {
		wcscpy(p, L"场地|");
		p += 3;
	}
	if(type & TYPE_COUNTER) {
		wcscpy(p, L"反击|");
		p += 3;
	}
	if(type & TYPE_FLIP) {
		wcscpy(p, L"反转|");
		p += 3;
	}
	if(type & TYPE_TOON) {
		wcscpy(p, L"卡通|");
		p += 3;
	}
	if(type & TYPE_XYZ) {
		wcscpy(p, L"超量|");
		p += 3;
	}
	if(p != tpBuffer)
		*(p - 1) = 0;
	else *p = 0;
	return tpBuffer;
}

}
