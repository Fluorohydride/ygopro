--キラーザウルス
function c80186010.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(80186010,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c80186010.cost)
	e1:SetTarget(c80186010.target)
	e1:SetOperation(c80186010.operation)
	c:RegisterEffect(e1)
end
function c80186010.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsAbleToGraveAsCost() and c:IsDiscardable() end
	Duel.SendtoGrave(c,REASON_COST+REASON_DISCARD)
end
function c80186010.filter(c)
	return c:GetCode()==10080320 and c:IsAbleToHand()
end
function c80186010.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return Duel.IsExistingMatchingCard(c80186010.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c80186010.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local tg=Duel.GetFirstMatchingCard(c80186010.filter,tp,LOCATION_DECK,0,nil)
	if tg then
		Duel.SendtoHand(tg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tg)
	end
end
