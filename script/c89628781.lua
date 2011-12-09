--忍法 空蝉の術
function c89628781.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c89628781.target)
	e1:SetOperation(c89628781.operation)
	c:RegisterEffect(e1)
end
function c89628781.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x2b)
end
function c89628781.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c89628781.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c89628781.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c89628781.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c89628781.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetRange(LOCATION_SZONE)
		e1:SetTargetRange(LOCATION_MZONE,0)
		e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetTarget(c89628781.indtg)
		e1:SetValue(1)
		c:RegisterEffect(e1)
	end
end
function c89628781.indtg(e,c)
	return e:GetHandler():GetFirstCardTarget()==c
end