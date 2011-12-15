--エクシーズ・バースト
function c30600344.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c30600344.condition)
	e1:SetTarget(c30600344.target)
	e1:SetOperation(c30600344.activate)
	c:RegisterEffect(e1)
end
function c30600344.cfilter(c)
	return c:IsFaceup() and c:IsRankAbove(6)
end
function c30600344.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c30600344.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c30600344.filter(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c30600344.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c30600344.filter,tp,0,LOCATION_ONFIELD,1,nil) end
	local g=Duel.GetMatchingGroup(c30600344.filter,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c30600344.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c30600344.filter,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
