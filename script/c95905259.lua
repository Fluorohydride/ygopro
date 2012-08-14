--予言僧 チョウレン
function c95905259.initial_effect(c)
	--confirm
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(95905259,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c95905259.target)
	e1:SetOperation(c95905259.operation)
	c:RegisterEffect(e1)
end
function c95905259.filter(c)
	return c:GetSequence()~=5 and c:IsFacedown()
end
function c95905259.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_SZONE) and c95905259.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c95905259.filter,tp,0,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(95905259,1))
	Duel.SelectTarget(tp,c95905259.filter,tp,0,LOCATION_SZONE,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CARDTYPE)
	local res=Duel.SelectOption(tp,71,72)
	e:SetLabel(res)
end
function c95905259.operation(e,tp,eg,ep,ev,re,r,rp)
	local res=e:GetLabel()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFacedown() then
		Duel.ConfirmCards(tp,tc)
		if (res==0 and tc:IsType(TYPE_SPELL)) or (res==1 and tc:IsType(TYPE_TRAP)) then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_TRIGGER)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e1)
		end
	end
end
