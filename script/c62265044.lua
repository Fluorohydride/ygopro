--竜の渓谷
function c62265044.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e2:SetDescription(aux.Stringid(62265044,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c62265044.cost)
	e2:SetTarget(c62265044.target)
	e2:SetOperation(c62265044.operation)
	c:RegisterEffect(e2)
end
function c62265044.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c62265044.filter1(c)
	return c:IsLevelBelow(4) and c:IsSetCard(0x29) and c:IsAbleToHand() 
end
function c62265044.filter2(c)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToGrave() 
end
function c62265044.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c62265044.filter1,tp,LOCATION_DECK,0,1,nil)
		or Duel.IsExistingMatchingCard(c62265044.filter2,tp,LOCATION_DECK,0,1,nil) end
	local b1=Duel.IsExistingMatchingCard(c62265044.filter1,tp,LOCATION_DECK,0,1,nil)
	local b2=Duel.IsExistingMatchingCard(c62265044.filter2,tp,LOCATION_DECK,0,1,nil)
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(62265044,0))
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(62265044,1),aux.Stringid(62265044,2))
	elseif b1 then Duel.SelectOption(tp,aux.Stringid(62265044,1)) op=0
	else Duel.SelectOption(tp,aux.Stringid(62265044,2)) op=1 end
	e:SetLabel(op)
	if op==0 then
		e:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
	else
		e:SetCategory(CATEGORY_TOGRAVE)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
	end
end
function c62265044.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if e:GetLabel()==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c62265044.filter1,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,c62265044.filter2,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoGrave(g,REASON_EFFECT)
		end
	end
end
