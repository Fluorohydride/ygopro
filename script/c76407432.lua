--バスター·カウンター
function c76407432.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c76407432.condition)
	e1:SetTarget(c76407432.target)
	e1:SetOperation(c76407432.activate)
	c:RegisterEffect(e1)
end
function c76407432.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x104f)
end
function c76407432.condition(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c76407432.cfilter,tp,LOCATION_MZONE,0,1,nil) then return false end
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if not Duel.IsChainInactivatable(ev) or loc==LOCATION_DECK then return false end
	local rc=re:GetHandler()
	return Duel.GetChainInfo(ev,CHAININFO_TYPE)==TYPE_MONSTER or re:IsHasType(EFFECT_TYPE_ACTIVATE)
end
function c76407432.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	local loc=eg:GetFirst():GetLocation()
	if eg:GetFirst():IsDestructable() and loc~=LOCATION_DECK then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c76407432.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	local loc=ec:GetLocation()
	if ec:IsRelateToEffect(e) and loc~=LOCATION_DECK then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
