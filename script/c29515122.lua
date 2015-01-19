--重機王ドボク・ザーク
function c29515122.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,5,3)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(29515122,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DECKDES)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c29515122.cost)
	e1:SetTarget(c29515122.target)
	e1:SetOperation(c29515122.operation)
	c:RegisterEffect(e1)
end
function c29515122.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c29515122.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(1-tp,3) end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,1-tp,3)
end
function c29515122.cfilter(c)
	return c:IsLocation(LOCATION_GRAVE) and c:IsType(TYPE_MONSTER)
end
function c29515122.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(1-tp,3,REASON_EFFECT)
	local g=Duel.GetOperatedGroup()
	local ct=g:FilterCount(c29515122.cfilter,nil)
	if ct==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local dg=Duel.SelectMatchingCard(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,ct,nil)
	Duel.Destroy(dg,REASON_EFFECT)
end
