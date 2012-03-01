--エレクトロ軍曹
function c43359262.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(43359262,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c43359262.target)
	e1:SetOperation(c43359262.operation)
	c:RegisterEffect(e1)
end
function c43359262.filter(c,rc)
	return c:IsFacedown() and c:GetSequence()~=5 and not rc:IsHasCardTarget(c)
end
function c43359262.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(1-tp) and c43359262.filter(chkc,e:GetHandler()) end
	if chk==0 then return Duel.IsExistingTarget(c43359262.filter,tp,0,LOCATION_SZONE,1,nil,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,515)
	Duel.SelectTarget(tp,c43359262.filter,tp,0,LOCATION_SZONE,1,1,nil,e:GetHandler())
end
function c43359262.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFacedown() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_TRIGGER)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c43359262.rcon)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
	end
end
function c43359262.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
