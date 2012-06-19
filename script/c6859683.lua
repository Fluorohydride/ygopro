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
function c6859683.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_EXTRA)>=2 end
end
function c6859683.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsType,1-tp,LOCATION_EXTRA,0,nil,TYPE_FUSION)
	local rg=g:RandomSelect(tp,2)
	Duel.SendtoGrave(rg,REASON_EFFECT)
	if rg:GetCount()<2 then
		local cg=Duel.GetFieldGroup(tp,0,LOCATION_EXTRA)
		Duel.ConfirmCards(tp,cg)
	end
end
