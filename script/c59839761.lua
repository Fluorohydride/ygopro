--デルタ・クロウ－アンチ・リバース
function c59839761.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c59839761.condition)
	e1:SetTarget(c59839761.target)
	e1:SetOperation(c59839761.activate)
	c:RegisterEffect(e1)
	--act in hand
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_TRAP_ACT_IN_HAND)
	e2:SetCondition(c59839761.handcon)
	c:RegisterEffect(e2)
end
function c59839761.handcon(e)
	return Duel.GetMatchingGroupCount(c59839761.cfilter,e:GetHandler():GetControler(),LOCATION_MZONE,0,nil)==3
end
function c59839761.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x33)
end
function c59839761.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c59839761.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c59839761.filter(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c59839761.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c59839761.filter,tp,0,LOCATION_SZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c59839761.filter,tp,0,LOCATION_SZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c59839761.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c59839761.filter,tp,0,LOCATION_SZONE,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
