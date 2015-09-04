--成功確率0%
function c6859683.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c6859683.target)
	e1:SetOperation(c6859683.operation)
	c:RegisterEffect(e1)
end
function c6859683.filter(c)
	return c:IsFacedown() or not c:IsType(TYPE_PENDULUM)
end
function c6859683.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_EXTRA)
	if chk==0 then return g:FilterCount(c6859683.filter,nil)>=2 end
end
function c6859683.operation(e,tp,eg,ep,ev,re,r,rp)
	local cg=Duel.GetFieldGroup(tp,0,LOCATION_EXTRA)
	Duel.ConfirmCards(tp,cg)
	local g=Duel.GetMatchingGroup(Card.IsType,1-tp,LOCATION_EXTRA,0,nil,TYPE_FUSION)
	if g:GetCount()<2 then return end
	local rg=g:RandomSelect(tp,2)
	Duel.SendtoGrave(rg,REASON_EFFECT)
end
