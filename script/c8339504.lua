--エクシーズ・シフト
function c8339504.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,8339504+EFFECT_COUNT_CODE_OATH)
	e1:SetLabel(0)
	e1:SetCost(c8339504.cost)
	e1:SetTarget(c8339504.target)
	e1:SetOperation(c8339504.activate)
	c:RegisterEffect(e1)
end
function c8339504.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	if chk==0 then return true end
end
function c8339504.cfilter(c,e,tp)
	local rk=c:GetRank()
	return rk>0 and Duel.IsExistingMatchingCard(c8339504.spfilter,tp,LOCATION_EXTRA,0,1,nil,rk,c:GetRace(),c:GetAttribute(),c:GetCode(),e,tp)
end
function c8339504.spfilter(c,rk,rc,att,code,e,tp)
	return c:GetRank()==rk and c:IsRace(rc) and c:IsAttribute(att) and not c:IsCode(code)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c8339504.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.CheckReleaseGroup(tp,c8339504.cfilter,1,nil,e,tp)
	end
	e:SetLabel(0)
	local g=Duel.SelectReleaseGroup(tp,c8339504.cfilter,1,1,nil,e,tp)
	Duel.Release(g,REASON_COST)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c8339504.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c8339504.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,tc:GetPreviousRankOnField(),tc:GetPreviousRaceOnField(),tc:GetPreviousAttributeOnField(),tc:GetPreviousCodeOnField(),e,tp)
	local sc=g:GetFirst()
	if sc then
		Duel.SpecialSummon(sc,0,tp,tp,false,false,POS_FACEUP)
		if c:IsRelateToEffect(e) then
			c:CancelToGrave()
			Duel.Overlay(sc,Group.FromCards(c))
		end
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetOperation(c8339504.tgop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		sc:RegisterEffect(e1)
	end
end
function c8339504.tgop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
end
