--エンド・オブ・アヌビス
function c65403020.initial_effect(c)
	--negate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAIN_SOLVING)
	e1:SetCondition(c65403020.condition)
	e1:SetOperation(c65403020.operation)
	c:RegisterEffect(e1)
end
function c65403020.gfilter(c)
	return c:IsLocation(LOCATION_GRAVE)
end
function c65403020.condition(e,tp,eg,ep,ev,re,r,rp)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if loc==LOCATION_GRAVE then return true end
	if not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	return g and g:IsExists(c65403020.gfilter,1,nil)
end
function c65403020.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateEffect(ev)
end
