--魔法の歯車
function c313513.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c313513.cost)
	e1:SetTarget(c313513.target)
	e1:SetOperation(c313513.activate)
	c:RegisterEffect(e1)
end
function c313513.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x7) and c:IsAbleToGraveAsCost()
end
function c313513.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c313513.cfilter,tp,LOCATION_ONFIELD,0,3,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c313513.cfilter,tp,LOCATION_ONFIELD,0,3,3,nil)
	Duel.SendtoGrave(g,REASON_COST)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetReset(RESET_SELF_TURN+RESET_PHASE+RESET_END,2)
	e1:SetTargetRange(1,0)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_MSET)
	Duel.RegisterEffect(e2,tp)
end
function c313513.filter(c,e,tp)
	return c:IsCode(83104731) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c313513.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c313513.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_DECK)
end
function c313513.dfilter(c)
	return c:IsFacedown() or c:GetCode()~=83104731
end
function c313513.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local ct=0
	if ft==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c313513.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,1,nil,e,tp)
		ct=Duel.SpecialSummon(g,0,tp,tp,true,false,POS_FACEUP)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g1=Duel.SelectMatchingCard(tp,c313513.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
		if g1:GetCount()>0 and Duel.SpecialSummonStep(g1:GetFirst(),0,tp,tp,true,false,POS_FACEUP) then ct=ct+1 end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g2=Duel.SelectMatchingCard(tp,c313513.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		if g2:GetCount()>0 and Duel.SpecialSummonStep(g2:GetFirst(),0,tp,tp,true,false,POS_FACEUP) then ct=ct+1 end
		Duel.SpecialSummonComplete()
	end
	if ct>0 then
		local dg=Duel.GetMatchingGroup(c313513.dfilter,tp,LOCATION_MZONE,0,nil)
		if dg:GetCount()>0 then
			Duel.BreakEffect()
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
