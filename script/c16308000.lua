--神の威光
function c16308000.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0x1c0,0x1c1)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c16308000.target)
	e1:SetOperation(c16308000.operation)
	c:RegisterEffect(e1)
end
function c16308000.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x4b)
end
function c16308000.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c16308000.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c16308000.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c16308000.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c16308000.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		e:SetLabelObject(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c16308000.rcon)
		e1:SetValue(aux.tgval)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
		e2:SetRange(LOCATION_SZONE)
		e2:SetCountLimit(1)
		e2:SetLabel(2)
		e2:SetLabelObject(tc)
		e2:SetOperation(c16308000.tgop)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN,2)
		c:RegisterEffect(e2)
	end
end
function c16308000.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
function c16308000.tgop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	local ct=e:GetLabel()
	ct=ct-1
	e:SetLabel(ct)
	if ct==0 and e:GetHandler():IsHasCardTarget(e:GetLabelObject()) then
		Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
	end
end
