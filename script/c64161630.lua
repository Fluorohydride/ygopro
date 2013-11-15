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
	e:SetLabel(1)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c64161630.cfilter,1,nil,e,tp) end
	local g=Duel.SelectReleaseGroup(tp,c64161630.cfilter,1,1,nil,e,tp)
	Duel.Release(g,REASON_COST)
	e:SetLabelObject(g:GetFirst())
end
function c64161630.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
	end
	e:SetLabel(0)
end
function c64161630.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetDescription(aux.Stringid(64161630,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetCountLimit(1)
	e1:SetTarget(c64161630.sptg)
	e1:SetOperation(c64161630.spop)
	e1:SetLabelObject(tc)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	tc:CreateEffectRelation(e1)
end
function c64161630.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c64161630.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=e:GetLabelObject()
	if not tc:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c64161630.filter,tp,LOCATION_EXTRA,0,1,1,nil,tc:GetRace()-1,tc:GetRace(),e,tp)
	Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
end
