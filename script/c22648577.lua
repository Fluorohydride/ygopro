--DDDの人事権
function c22648577.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c22648577.target)
	e1:SetOperation(c22648577.operation)
	c:RegisterEffect(e1)
end
function c22648577.filter(c)
	if c:IsLocation(LOCATION_MZONE) and c:IsFacedown() then return false end
	if c:IsLocation(LOCATION_SZONE) then
		if c:GetSequence()<6 then return false end
	elseif not c:IsType(TYPE_MONSTER) then return false end
	return c:IsSetCard(0xaf) and c:IsAbleToDeck()
end
function c22648577.thfilter(c)
	return c:IsSetCard(0xaf) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c22648577.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c22648577.filter,tp,LOCATION_ONFIELD+LOCATION_GRAVE+LOCATION_HAND,0,3,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,3,tp,LOCATION_ONFIELD+LOCATION_GRAVE+LOCATION_HAND)
end
function c22648577.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c22648577.filter,tp,LOCATION_ONFIELD+LOCATION_GRAVE+LOCATION_HAND,0,nil)
	if g:GetCount()<3 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local sg=g:Select(tp,3,3,nil)
	local cg=sg:Filter(Card.IsLocation,nil,LOCATION_HAND)
	Duel.ConfirmCards(1-tp,cg)
	Duel.SendtoDeck(sg,nil,0,REASON_EFFECT)
	local dg=Duel.GetMatchingGroup(c22648577.thfilter,tp,LOCATION_DECK,0,nil)
	if dg:GetCount()>1 and Duel.SelectYesNo(tp,aux.Stringid(22648577,0)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local tg=dg:Select(tp,2,2,nil)
		Duel.SendtoHand(tg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tg)
	end
end
