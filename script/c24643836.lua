--おジャマジック
function c24643836.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(24643836,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c24643836.thcon)
	e1:SetTarget(c24643836.thtg)
	e1:SetOperation(c24643836.thop)
	c:RegisterEffect(e1)
end
function c24643836.thcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD+LOCATION_HAND)
end
function c24643836.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,3,tp,LOCATION_DECK)
end
function c24643836.filter(c,code)
	return c:IsCode(code) and c:IsAbleToHand()
end
function c24643836.thop(e,tp,eg,ep,ev,re,r,rp)
	local t1=Duel.GetFirstMatchingCard(c24643836.filter,tp,LOCATION_DECK,0,nil,12482652)
	if not t1 then return end
	local t2=Duel.GetFirstMatchingCard(c24643836.filter,tp,LOCATION_DECK,0,nil,42941100)
	if not t2 then return end
	local t3=Duel.GetFirstMatchingCard(c24643836.filter,tp,LOCATION_DECK,0,nil,79335209)
	if not t3 then return end
	local g=Group.FromCards(t1,t2,t3)
	Duel.SendtoHand(g,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,g)
end
