--召喚制限－ディスコードセクター
function c47594939.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--disable spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,1)
	e2:SetTarget(c47594939.splimit)
	c:RegisterEffect(e2)
end
function c47594939.lvfilter(c,lv)
	return c:GetLevel()==lv
end
function c47594939.rkfilter(c,rk)
	return c:GetRank()==rk
end
function c47594939.splimit(e,c,sump,sumtype,sumpos,targetp)
	local lv=c:GetLevel()
	local rk=c:GetRank()
	if lv>0 then
		return Duel.IsExistingMatchingCard(c47594939.lvfilter,sump,LOCATION_MZONE,0,1,nil,lv)
	end
	return Duel.IsExistingMatchingCard(c47594939.rkfilter,sump,LOCATION_MZONE,0,1,nil,rk)
end
