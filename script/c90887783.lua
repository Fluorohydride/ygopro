--竜の交感
function c90887783.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c90887783.target)
	e1:SetOperation(c90887783.operation)
	c:RegisterEffect(e1)
end
function c90887783.filter1(c,tp)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToDeck() and not c:IsPublic()
		and Duel.IsExistingMatchingCard(c90887783.filter2,tp,LOCATION_DECK,0,1,nil,c:GetLevel())
end
function c90887783.filter2(c,lv)
	return c:IsRace(RACE_DRAGON) and c:GetLevel()==lv and c:IsAbleToHand()
end
function c90887783.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c90887783.filter1,tp,LOCATION_HAND,0,1,nil,tp) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,1,tp,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c90887783.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g1=Duel.SelectMatchingCard(tp,c90887783.filter1,tp,LOCATION_HAND,0,1,1,nil,tp)
	Duel.ConfirmCards(1-tp,g1)
	if g1:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g2=Duel.SelectMatchingCard(tp,c90887783.filter2,tp,LOCATION_DECK,0,1,1,nil,g1:GetFirst():GetLevel())
	Duel.SendtoHand(g2,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,g2)
	Duel.SendtoDeck(g1,nil,2,REASON_EFFECT)
end
