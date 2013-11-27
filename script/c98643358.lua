--ダイヤモンド・ダスト
function c98643358.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetTarget(c98643358.target)
	e1:SetOperation(c98643358.activate)
	c:RegisterEffect(e1)
end
function c98643358.filter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER) and c:IsDestructable()
end
function c98643358.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c98643358.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c98643358.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetCount()*500)
end
function c98643358.ctfilter(c)
	return c:IsLocation(LOCATION_GRAVE) and c:IsAttribute(ATTRIBUTE_WATER)
end
function c98643358.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c98643358.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if Duel.Destroy(g,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		local ct=Duel.GetOperatedGroup():FilterCount(c98643358.ctfilter,nil)
		Duel.Damage(1-tp,ct*500,REASON_EFFECT)
	end
end
