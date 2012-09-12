--オーディンの眼
function c88069166.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--confirm
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(88069166,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetTarget(c88069166.target)
	e2:SetOperation(c88069166.operation)
	c:RegisterEffect(e2)
	--cancel target
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetOperation(c88069166.ctarget)
	c:RegisterEffect(e3)
end
function c88069166.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x4b)
end
function c88069166.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c88069166.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c88069166.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c88069166.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetChainLimit(c88069166.chlimit)
end
function c88069166.chlimit(e,ep,tp)
	return tp==ep
end
function c88069166.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		e:SetLabelObject(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetCondition(c88069166.rcon)
		tc:RegisterEffect(e1)
		local g1=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
		local g2=Duel.GetMatchingGroup(Card.IsFacedown,tp,0,LOCATION_ONFIELD,nil)
		g1:Merge(g2)
		Duel.ConfirmCards(tp,g1)
		Duel.ShuffleHand(1-tp)
	end
end
function c88069166.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
function c88069166.ctarget(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	if tc then e:GetHandler():CancelCardTarget(tc) end
end
