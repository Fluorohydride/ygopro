--バスター・スラッシュ
function c40012727.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c40012727.condition)
	e1:SetTarget(c40012727.target)
	e1:SetOperation(c40012727.activate)
	c:RegisterEffect(e1)
end
function c40012727.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x104f)
end
function c40012727.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c40012727.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c40012727.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c40012727.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c40012727.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local dg=Duel.GetMatchingGroup(c40012727.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c40012727.activate(e,tp,eg,ep,ev,re,r,rp)
	local dg=Duel.GetMatchingGroup(c40012727.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.Destroy(dg,REASON_EFFECT)
end
