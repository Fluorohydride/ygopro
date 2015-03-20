--墓場からの呼び声
function c16970158.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c16970158.condition)
	e1:SetTarget(c16970158.target)
	e1:SetOperation(c16970158.activate)
	c:RegisterEffect(e1)
end
function c16970158.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetHandler():IsCode(83764718) and Duel.IsChainDisablable(ev)
end
function c16970158.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
end
function c16970158.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateEffect(ev)
end
