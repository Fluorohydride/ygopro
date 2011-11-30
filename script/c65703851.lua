--透破抜き
function c65703851.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c65703851.condition)
	e1:SetTarget(c65703851.target)
	e1:SetOperation(c65703851.activate)
	c:RegisterEffect(e1)
end
function c65703851.condition(e,tp,eg,ep,ev,re,r,rp)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	return (loc==LOCATION_HAND or loc==LOCATION_GRAVE) and Duel.GetChainInfo(ev,CHAININFO_TYPE)==TYPE_MONSTER and Duel.IsChainInactivatable(ev)
end
function c65703851.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	local cloc=eg:GetFirst():GetLocation()
	if loc==cloc then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_REMOVE,eg,1,0,0)
	end
end
function c65703851.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if eg:GetFirst():IsRelateToEffect(e) then
		Duel.Remove(eg,POS_FACEUP,REASON_EFFECT)
	end
end
