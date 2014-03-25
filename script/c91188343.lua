--神秘の代行者 アース
function c91188343.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(91188343,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c91188343.tg)
	e1:SetOperation(c91188343.op)
	c:RegisterEffect(e1)
end
function c91188343.filter1(c)
	return c:IsSetCard(0x44) and c:GetCode()~=91188343 and c:IsAbleToHand()
end
function c91188343.filter2(c)
	return ((c:IsSetCard(0x44) and c:GetCode()~=91188343) or c:GetCode()==55794644) and c:IsAbleToHand()
end
function c91188343.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not Duel.IsEnvironment(56433456) then
			return Duel.IsExistingMatchingCard(c91188343.filter1,tp,LOCATION_DECK,0,1,nil) end
		return Duel.IsExistingMatchingCard(c91188343.filter2,tp,LOCATION_DECK,0,1,nil)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c91188343.op(e,tp,eg,ep,ev,re,r,rp)
	local g=nil
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	if not Duel.IsEnvironment(56433456) then
		g=Duel.SelectMatchingCard(tp,c91188343.filter1,tp,LOCATION_DECK,0,1,1,nil)
	else g=Duel.SelectMatchingCard(tp,c91188343.filter2,tp,LOCATION_DECK,0,1,1,nil) end
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
