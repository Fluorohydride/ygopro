--パワー・ブレイク
function c86821010.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c86821010.condition)
	e1:SetTarget(c86821010.target)
	e1:SetOperation(c86821010.activate)
	c:RegisterEffect(e1)
end
function c86821010.cfilter(c)
	return c:IsFaceup() and c:IsCode(2403771)
end
function c86821010.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c86821010.cfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c86821010.filter(c)
	return c:IsType(TYPE_EQUIP) and c:IsAbleToDeck()
end
function c86821010.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE+LOCATION_GRAVE) and chkc:IsControler(tp) and c86821010.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c86821010.filter,tp,LOCATION_SZONE+LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c86821010.filter,tp,LOCATION_SZONE+LOCATION_GRAVE,0,1,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetCount()*500)
end
function c86821010.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	local g=Duel.GetOperatedGroup()
	local ct=g:FilterCount(Card.IsLocation,nil,LOCATION_DECK+LOCATION_EXTRA)
	if ct>0 then
		Duel.Damage(1-tp,ct*500,REASON_EFFECT)
	end
end
