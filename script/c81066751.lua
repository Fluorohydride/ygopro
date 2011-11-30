--神罰
function c81066751.initial_effect(c)
	--Activate(effect)
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e4:SetType(EFFECT_TYPE_ACTIVATE)
	e4:SetCode(EVENT_CHAINING)
	e4:SetCondition(c81066751.condition)
	e4:SetTarget(c81066751.target)
	e4:SetOperation(c81066751.activate)
	c:RegisterEffect(e4)
end
function c81066751.condition(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetEnvironment()~=56433456 then return false end
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if (not Duel.IsChainInactivatable(ev)) or loc==LOCATION_DECK then return false end
	local rc=re:GetHandler()
	return Duel.GetChainInfo(ev,CHAININFO_TYPE)==TYPE_MONSTER or re:IsHasType(EFFECT_TYPE_ACTIVATE)
end
function c81066751.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	local loc=eg:GetFirst():GetLocation()
	if eg:GetFirst():IsDestructable() and loc~=LOCATION_DECK then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c81066751.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	local loc=ec:GetLocation()
	if ec:IsRelateToEffect(e) and loc~=LOCATION_DECK then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
