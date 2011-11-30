--自由解放
function c72022087.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c72022087.condition)
	e1:SetTarget(c72022087.target)
	e1:SetOperation(c72022087.activate)
	c:RegisterEffect(e1)
end
function c72022087.cfilter(c,tp)
	return c:IsReason(REASON_BATTLE) and c:IsLocation(LOCATION_GRAVE) and c:GetPreviousControler()==tp
end
function c72022087.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c72022087.cfilter,1,nil,tp)
end
function c72022087.filter(c)
	return c:IsFaceup() and c:IsAbleToDeck()
end
function c72022087.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c72022087.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c72022087.filter,tp,LOCATION_MZONE,LOCATION_MZONE,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp, c72022087.filter,tp,LOCATION_MZONE,LOCATION_MZONE,2,2,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,2,0,0)
end
function c72022087.acfilter(c,e)
	return c:IsFaceup() and c:IsRelateToEffect(e)
end
function c72022087.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c72022087.acfilter,nil,e)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
