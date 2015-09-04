--ガーベージ・オーガ
function c18698739.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(18698739,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,18698739)
	e1:SetCost(c18698739.cost)
	e1:SetTarget(c18698739.target)
	e1:SetOperation(c18698739.operation)
	c:RegisterEffect(e1)
end
function c18698739.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c18698739.filter(c)
	return c:GetCode()==44682448 and c:IsAbleToHand()
end
function c18698739.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c18698739.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c18698739.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=Duel.GetFirstMatchingCard(c18698739.filter,tp,LOCATION_DECK,0,nil)
	if tc then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
