--フュージョン・ガード
function c73026394.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c73026394.condition)
	e1:SetTarget(c73026394.target)
	e1:SetOperation(c73026394.activate)
	c:RegisterEffect(e1)
end
function c73026394.condition(e,tp,eg,ep,ev,re,r,rp)
	local ex=Duel.GetOperationInfo(ev,CATEGORY_DAMAGE)
	return ex and (re:IsActiveType(TYPE_MONSTER) or re:IsHasType(EFFECT_TYPE_ACTIVATE)) and Duel.IsChainNegatable(ev)
end
function c73026394.filter(c)
	return c:IsType(TYPE_FUSION) and c:IsAbleToGrave()
end
function c73026394.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c73026394.filter,tp,LOCATION_EXTRA,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_EXTRA)
end
function c73026394.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local g=Duel.GetMatchingGroup(c73026394.filter,tp,LOCATION_EXTRA,0,nil):RandomSelect(tp,1)
	Duel.SendtoGrave(g,REASON_EFFECT)
end
