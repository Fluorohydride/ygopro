--フォトン・ブースター
function c71233859.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c71233859.target)
	e1:SetOperation(c71233859.activate)
	c:RegisterEffect(e1)
end
function c71233859.filter(c)
	return c:IsFaceup() and not c:IsType(TYPE_TOKEN) and c:IsLevelBelow(4) and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c71233859.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c71233859.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c71233859.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c71233859.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c71233859.afilter(c,code)
	return c:IsFaceup() and c:IsCode(code)
end
function c71233859.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFacedown() or not tc:IsRelateToEffect(e) then return end
	local g=Duel.GetMatchingGroup(c71233859.afilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tc:GetCode())
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		e1:SetValue(2000)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end
