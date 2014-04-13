--ストレートフラッシュ
function c25173686.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c25173686.condition)
	e1:SetTarget(c25173686.target)
	e1:SetOperation(c25173686.activate)
	c:RegisterEffect(e1)
end
function c25173686.condition(e,tp,eg,ep,ev,re,r,rp)
	for i=0,4 do
		if Duel.GetFieldCard(1-tp,LOCATION_SZONE,i)==nil then return false end
	end
	return true
end
function c25173686.filter(c)
	return c:GetSequence()<5 and c:IsDestructable()
end
function c25173686.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c25173686.filter,tp,0,LOCATION_SZONE,1,nil) end
	local sg=Duel.GetMatchingGroup(c25173686.filter,tp,0,LOCATION_SZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,sg,sg:GetCount(),0,0)
end
function c25173686.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(c25173686.filter,tp,0,LOCATION_SZONE,nil)
	Duel.Destroy(sg,REASON_EFFECT)
end
