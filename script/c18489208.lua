--カースド・フィグ
function c18489208.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(18489208,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c18489208.con)
	e1:SetTarget(c18489208.tg)
	e1:SetOperation(c18489208.op)
	c:RegisterEffect(e1)
end
function c18489208.con(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c18489208.tg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsFacedown() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFacedown,tp,LOCATION_SZONE,LOCATION_SZONE,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEDOWN)
	Duel.SelectTarget(tp,Card.IsFacedown,tp,LOCATION_SZONE,LOCATION_SZONE,2,2,nil)
end
function c18489208.op(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsLocation(LOCATION_GRAVE) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	while tc do
		if c:IsRelateToEffect(e) and tc:IsFacedown() and tc:IsRelateToEffect(e) then
			c:SetCardTarget(tc)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_TRIGGER)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			e1:SetCondition(c18489208.rcon)
			e1:SetValue(1)
			tc:RegisterEffect(e1)
		end
		tc=g:GetNext()
	end
end
function c18489208.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
