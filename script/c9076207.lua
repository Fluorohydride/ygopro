--青い忍者
function c9076207.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(9076207,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c9076207.target)
	e1:SetOperation(c9076207.operation)
	c:RegisterEffect(e1)
end
function c9076207.filter(c)
	return c:IsFacedown() or c:IsType(TYPE_SPELL)
end
function c9076207.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c9076207.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c9076207.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	if g:GetCount()>0 and g:GetFirst():IsFaceup() then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c9076207.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(tp,tc) end
		if tc:IsType(TYPE_SPELL) then Duel.Destroy(tc,REASON_EFFECT) end
	end
end
