--弱肉一色
function c66926224.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c66926224.condition)
	e1:SetTarget(c66926224.target)
	e1:SetOperation(c66926224.activate)
	c:RegisterEffect(e1)
end
function c66926224.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_NORMAL) and c:IsLevelBelow(2)
end
function c66926224.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c66926224.cfilter,tp,LOCATION_MZONE,0,5,nil)
end
function c66926224.dfilter(c)
	return not (c:IsFaceup() and c:IsType(TYPE_NORMAL) and c:IsLevelBelow(2)) and c:IsDestructable()
end
function c66926224.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.IsExistingMatchingCard(nil,tp,LOCATION_HAND,LOCATION_HAND,1,c)
		or Duel.IsExistingMatchingCard(c66926224.dfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,c) end
	local g=Duel.GetMatchingGroup(c66926224.dfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,c)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,PLAYER_ALL,1)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c66926224.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,LOCATION_HAND,LOCATION_HAND)
	Duel.SendtoGrave(g1,REASON_EFFECT+REASON_DISCARD)
	local g2=Duel.GetMatchingGroup(c66926224.dfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.Destroy(g2,REASON_EFFECT)
end
