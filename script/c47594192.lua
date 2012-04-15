--スリーカード
function c47594192.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c47594192.condition)
	e1:SetTarget(c47594192.target)
	e1:SetOperation(c47594192.activate)
	c:RegisterEffect(e1)
end
function c47594192.cfilter(c,tp)
	return c:IsFaceup() and not c:IsType(TYPE_TOKEN) and Duel.IsExistingMatchingCard(c47594192.cfilter2,tp,LOCATION_MZONE,0,2,c,c:GetCode())
end
function c47594192.cfilter2(c,code)
	return c:IsFaceup() and c:GetCode()==code
end
function c47594192.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c47594192.cfilter,tp,LOCATION_MZONE,0,1,nil,tp)
end
function c47594192.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_ONFIELD,3,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,3,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,3,0,0)
end
function c47594192.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
