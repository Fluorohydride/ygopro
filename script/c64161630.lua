--パージ・レイ
function c64161630.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c64161630.cost)
	e1:SetTarget(c64161630.target)
	e1:SetOperation(c64161630.activate)
	c:RegisterEffect(e1)
end
function c64161630.cfilter(c,e,tp)
	local rk=c:GetRank()
	return rk>1 and c:IsType(TYPE_XYZ)
		and Duel.IsExistingMatchingCard(c64161630.filter,tp,LOCATION_EXTRA,0,1,nil,rk-1,c:GetRace(),e,tp)
end
function c64161630.filter(c,rk,rc,e,tp)
	return c:IsType(TYPE_XYZ) and c:IsSetCard(0x48)
		and c:GetRank()==rk and c:IsRace(rc) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c64161630.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c64161630.cfilter,1,nil,e,tp) end
	local g=Duel.SelectReleaseGroup(tp,c64161630.cfilter,1,1,nil,e,tp)
	e:SetLabel(g:GetFirst():GetRank())
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetLabel(g:GetFirst():GetRace())
	Duel.RegisterEffect(e1,tp)
	e:SetLabelObject(e1)
	Duel.Release(g,REASON_COST)
end
function c64161630.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
	end
end
function c64161630.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetOperation(c64161630.spop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetLabel(e:GetLabel())
	e1:SetLabelObject(e:GetLabelObject())
	Duel.RegisterEffect(e1,tp)
end
function c64161630.spop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_CARD,0,64161630)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local rk=e:GetLabel()
	local rc=e:GetLabelObject():GetLabel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c64161630.filter,tp,LOCATION_EXTRA,0,1,1,nil,rk-1,rc,e,tp)
	Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
end
