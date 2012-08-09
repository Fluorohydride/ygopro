--嵐
function c13210191.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c13210191.target)
	e1:SetOperation(c13210191.activate)
	c:RegisterEffect(e1)
end
function c13210191.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c13210191.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.IsExistingMatchingCard(c13210191.filter,tp,LOCATION_ONFIELD,0,1,c) end
	local g1=Duel.GetMatchingGroup(c13210191.filter,tp,LOCATION_ONFIELD,0,c)
	local g2=Duel.GetMatchingGroup(c13210191.filter,tp,0,LOCATION_ONFIELD,nil)
	local ct1=g1:GetCount()
	local ct2=g2:GetCount()
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,ct1+((ct1>ct2) and ct2 or ct1),0,0)
end
function c13210191.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(c13210191.filter,tp,LOCATION_ONFIELD,0,e:GetHandler())
	local ct1=Duel.Destroy(g1,REASON_EFFECT)
	if ct1==0 then return end
	local g2=Duel.GetMatchingGroup(c13210191.filter,tp,0,LOCATION_ONFIELD,nil)
	if g2:GetCount()<=ct1 then
		Duel.Destroy(g2,REASON_EFFECT)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g3=g2:Select(tp,ct1,ct1,nil)
		Duel.HintSelection(g3)
		Duel.Destroy(g3,REASON_EFFECT)
	end
end
