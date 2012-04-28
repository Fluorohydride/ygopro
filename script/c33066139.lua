--カードを狩る死神
function c33066139.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(33066139,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c33066139.target)
	e1:SetOperation(c33066139.operation)
	c:RegisterEffect(e1)
end
function c33066139.filter(c)
	return c:IsFacedown() or c:IsType(TYPE_TRAP)
end
function c33066139.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c33066139.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c33066139.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,e:GetHandler())
	if g:GetCount()>0 and g:GetFirst():IsFaceup() then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c33066139.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(tp,tc) end
		if tc:IsType(TYPE_TRAP) then Duel.Destroy(tc,REASON_EFFECT) end
	end
end
