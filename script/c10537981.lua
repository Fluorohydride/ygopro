--リターンソウル
function c10537981.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_END_PHASE)
	e1:SetCondition(c10537981.condition)
	e1:SetTarget(c10537981.target)
	e1:SetOperation(c10537981.activate)
	c:RegisterEffect(e1)
end
function c10537981.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_END
end
function c10537981.filter(c,tid)
	return c:IsReason(REASON_DESTROY) and c:IsType(TYPE_MONSTER) and c:GetTurnID()==tid
		and c:IsAbleToDeck()
end
function c10537981.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and c10537981.filter(chkc,Duel.GetTurnCount()) end
	if chk==0 then return Duel.IsExistingTarget(c10537981.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil,Duel.GetTurnCount()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c10537981.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,3,nil,Duel.GetTurnCount())
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c10537981.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()>0 then
		Duel.SendtoDeck(sg,nil,2,REASON_EFFECT)
	end
end
