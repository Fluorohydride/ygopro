--融合準備
function c66127916.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c66127916.target)
	e1:SetOperation(c66127916.activate)
	c:RegisterEffect(e1)
end
function c66127916.filter1(c,tp)
	return c.material_count and Duel.IsExistingMatchingCard(c66127916.filter2,tp,LOCATION_DECK,0,1,nil,c)
end
function c66127916.filter2(c,fc)
	if c:IsHasEffect(EFFECT_FORBIDDEN) or not c:IsAbleToHand() then return false end
	for i=1,fc.material_count do
		if c:IsCode(fc.material[i]) then return true end
	end
	return false
end
function c66127916.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c66127916.filter1,tp,LOCATION_EXTRA,0,1,nil,tp) end
end
function c66127916.filter3(c)
	return c:IsCode(24094653) and c:IsAbleToHand()
end
function c66127916.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local cg=Duel.SelectMatchingCard(tp,c66127916.filter1,tp,LOCATION_EXTRA,0,1,1,nil,tp)
	if cg:GetCount()==0 then return end
	Duel.ConfirmCards(1-tp,cg)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c66127916.filter2,tp,LOCATION_DECK,0,1,1,nil,cg:GetFirst())
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
		local tg=Duel.GetMatchingGroup(c66127916.filter3,tp,LOCATION_GRAVE,0,nil)
		if tg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(66127916,0)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local sg=tg:Select(tp,1,1,nil)
			Duel.SendtoHand(sg,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,sg)
		end
	end
end
