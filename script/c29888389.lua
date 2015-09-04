--シャドウ・リチュア
function c29888389.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(29888389,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c29888389.cost)
	e1:SetTarget(c29888389.target)
	e1:SetOperation(c29888389.operation)
	c:RegisterEffect(e1)
	--ritual level
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_RITUAL_LEVEL)
	e2:SetValue(c29888389.rlevel)
	c:RegisterEffect(e2)
end
function c29888389.rlevel(e,c)
	local lv=e:GetHandler():GetLevel()
	if c:IsAttribute(ATTRIBUTE_WATER) then
		local clv=c:GetLevel()
		return lv*65536+clv
	else return lv end
end
function c29888389.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsDiscardable() end
	Duel.SendtoGrave(c,REASON_COST+REASON_DISCARD)
end
function c29888389.filter(c)
	return c:IsSetCard(0x3a) and c:GetType()==TYPE_SPELL+TYPE_RITUAL and c:IsAbleToHand()
end
function c29888389.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return Duel.IsExistingMatchingCard(c29888389.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c29888389.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c29888389.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
