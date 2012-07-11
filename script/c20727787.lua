--武装解除
function c20727787.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_EQUIP)
	e1:SetTarget(c20727787.target)
	e1:SetOperation(c20727787.activate)
	c:RegisterEffect(e1)
end
function c20727787.filter(c)
	return c:IsType(TYPE_EQUIP) and c:IsDestructable()
end
function c20727787.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.IsExistingMatchingCard(c20727787.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,c) end
	local g=Duel.GetMatchingGroup(c20727787.filter,tp,LOCATION_SZONE,LOCATION_SZONE,c)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c20727787.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c20727787.filter,tp,LOCATION_SZONE,LOCATION_SZONE,e:GetHandler())
	Duel.Destroy(g,REASON_EFFECT)
end
