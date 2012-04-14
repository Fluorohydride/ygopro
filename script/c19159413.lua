--魔法除去
function c19159413.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c19159413.target)
	e1:SetOperation(c19159413.activate)
	c:RegisterEffect(e1)
end
function c19159413.filter(c)
	return c:IsFacedown() or c:IsType(TYPE_SPELL)
end
function c19159413.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c19159413.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c19159413.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c19159413.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,e:GetHandler())
	if g:GetFirst():IsFaceup() then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c19159413.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(tp,tc) end
		if tc:IsType(TYPE_SPELL) then Duel.Destroy(tc,REASON_EFFECT) end
	end
end
