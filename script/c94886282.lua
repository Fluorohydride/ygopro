--光の援軍
function c94886282.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c94886282.cost)
	e1:SetTarget(c94886282.target)
	e1:SetOperation(c94886282.activate)
	c:RegisterEffect(e1)
end
function c94886282.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeckAsCost(tp,3) end
	Duel.DiscardDeck(tp,3,REASON_COST)
end
function c94886282.filter(c)
	return c:IsSetCard(0x38) and c:IsLevelBelow(4) and c:IsAbleToHand()
end
function c94886282.orifilter(c)
	return c:IsSetCard(0x38) and c:IsLevelBelow(4)
end
function c94886282.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c94886282.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c94886282.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c94886282.filter,tp,LOCATION_DECK,0,1,1,nil)
	local cg=Duel.GetFieldGroup(tp,LOCATION_DECK,0)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	elseif not Duel.IsExistingMatchingCard(c94886282.orifilter,tp,LOCATION_DECK,0,1,nil) and cg:FilterCount(Card.IsAbleToHand,nil)>0 then
		Duel.ConfirmCards(1-tp,cg)
		Duel.ShuffleDeck(tp)
	end
end
