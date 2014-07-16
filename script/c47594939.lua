--召喚制限－ディスコードセクター
function c47594939.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c47594939.adjustop)
	c:RegisterEffect(e2)
	--disable spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(1,1)
	e3:SetTarget(c47594939.splimit)
	c:RegisterEffect(e3)
	local g=Group.CreateGroup()
	g:KeepAlive()
	e2:SetLabelObject(g)
	e3:SetLabelObject(g)
end
function c47594939.lvfilter(c,lv,tp)
	return c:GetLevel()==lv and c:IsControler(tp)
end
function c47594939.rkfilter(c,rk,tp)
	return c:GetRank()==rk and c:IsControler(tp)
end
function c47594939.splimit(e,c,sump,sumtype,sumpos,targetp)
	local lv=c:GetLevel()
	local rk=c:GetRank()
	if lv>0 then
		return e:GetLabelObject():IsExists(c47594939.lvfilter,1,nil,lv,sump)
	end
	return e:GetLabelObject():IsExists(c47594939.rkfilter,1,nil,rk,sump)
end
function c47594939.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase==PHASE_DAMAGE and not Duel.IsDamageCalculated()) or phase==PHASE_DAMAGE_CAL then return end
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	e:GetLabelObject():Clear()
	e:GetLabelObject():Merge(g)
end
