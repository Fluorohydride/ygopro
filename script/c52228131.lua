--パリィ
function c52228131.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c52228131.condition)
	e1:SetTarget(c52228131.target)
	e1:SetOperation(c52228131.activate)
	c:RegisterEffect(e1)
end
function c52228131.filter(c)
	return c:IsSetCard(0x19) and c:IsAbleToDeck()
end
function c52228131.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.GetChainInfo(ev,CHAININFO_TYPE)==TYPE_TRAP and Duel.IsChainInactivatable(ev)
end
function c52228131.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c98891840.filter,tp,LOCATION_HAND,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if eg:GetFirst():IsDestructable() then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c52228131.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectMatchingCard(tp,c98891840.filter,p,LOCATION_HAND,0,1,1,nil)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	if ec:IsRelateToEffect(e) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
