--融合準備
function c80200078.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(80200078,0))
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c80200078.coscost)
	e1:SetOperation(c80200078.cosoperation)
	c:RegisterEffect(e1)
end
function c80200078.filter2(c,fc)
	local fd=c:GetCode()
	for i=1,fc.material_count do
		if fd==fc.material[i] then return true end
	end
	return false
end
function c80200078.filter1(c,tp)
	local ct=c.material_count
	return ct~=nil and Duel.IsExistingMatchingCard(c80200078.filter2,tp,LOCATION_DECK,0,1,nil,c)
end
function c80200078.coscost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c80200078.filter1,tp,LOCATION_EXTRA,0,1,nil,tp) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c80200078.cosoperation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local g=Duel.SelectMatchingCard(tp,c80200078.filter1,tp,LOCATION_EXTRA,0,1,1,nil,tp)
	if g:GetCount()>0 then 
		Duel.ConfirmCards(1-tp,g)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local cg=Duel.SelectMatchingCard(tp,c80200078.filter2,tp,LOCATION_DECK,0,1,1,nil,g:GetFirst())
		
		if cg:GetCount()>0 and Duel.SendtoHand(cg,nil,REASON_EFFECT) then
			Duel.ConfirmCards(1-tp,cg)
			
			local g1=Duel.GetMatchingGroup(Card.IsCode,tp,LOCATION_GRAVE,0,nil,24094653)
			if g1:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(80200078,0)) then
				Duel.BreakEffect()
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
				local sg=g1:Select(tp,1,1,nil)
				Duel.SendtoHand(sg,nil,REASON_EFFECT)
				Duel.ConfirmCards(1-tp,sg)
			end
		end
	end
end
