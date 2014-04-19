--連鎖解呪
function c3171055.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c3171055.condition)
	e1:SetOperation(c3171055.activate)
	c:RegisterEffect(e1)
end
function c3171055.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE)
end
function c3171055.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsCode,ep,LOCATION_DECK,0,nil,re:GetHandler():GetCode())
	Duel.Destroy(g,REASON_EFFECT)
end
