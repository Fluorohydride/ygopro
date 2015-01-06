--おジャマ・デルタハリケーン！！
function c8251996.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c8251996.condition)
	e1:SetTarget(c8251996.target)
	e1:SetOperation(c8251996.activate)
	c:RegisterEffect(e1)
end
function c8251996.cfilter(c,code)
	return c:IsFaceup() and c:IsCode(code)
end
function c8251996.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c8251996.cfilter,tp,LOCATION_MZONE,0,1,nil,12482652)
		and Duel.IsExistingMatchingCard(c8251996.cfilter,tp,LOCATION_MZONE,0,1,nil,42941100)
		and Duel.IsExistingMatchingCard(c8251996.cfilter,tp,LOCATION_MZONE,0,1,nil,79335209)
end
function c8251996.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c8251996.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
