--アビス・ウォリアー
function c88409165.initial_effect(c)
	--to hand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(88409165,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c88409165.cost)
	e1:SetTarget(c88409165.target)
	e1:SetOperation(c88409165.operation)
	c:RegisterEffect(e1)
end
function c88409165.cfilter(c)
	return c:IsAttribute(ATTRIBUTE_WATER) and c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c88409165.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c88409165.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c88409165.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c88409165.filter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToDeck()
end
function c88409165.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and c88409165.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c88409165.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c88409165.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c88409165.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		if tc:IsType(TYPE_FUSION+TYPE_SYNCHRO+TYPE_XYZ)
			or Duel.SelectOption(tp,aux.Stringid(88409165,1),aux.Stringid(88409165,2))==0 then
			Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
		else
			Duel.SendtoDeck(tc,nil,1,REASON_EFFECT)
		end
	end
end
