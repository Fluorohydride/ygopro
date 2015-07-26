--猪突猛進
function c48422921.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c48422921.condition)
	e1:SetTarget(c48422921.target)
	e1:SetOperation(c48422921.operation)
	c:RegisterEffect(e1)
end
function c48422921.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsAbleToEnterBP() or Duel.GetCurrentPhase()==PHASE_BATTLE
end
function c48422921.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_MZONE,0,1,1,nil)
	local val=0xff
	local reg=g:GetFirst():GetFlagEffectLabel(48422921)
	if reg then val=val-reg end
	Duel.Hint(HINT_SELECTMSG,tp,562)
	local att=Duel.AnnounceAttribute(tp,1,val)
	e:SetLabel(att)
end
function c48422921.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local att=e:GetLabel()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_BATTLE_START)
		e1:SetLabel(att)
		e1:SetOwnerPlayer(tp)
		e1:SetCondition(c48422921.descon)
		e1:SetOperation(c48422921.desop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1,true)
		local reg=tc:GetFlagEffectLabel(48422921)
		if reg then
			reg=bit.bor(reg,att)
			tc:SetFlagEffectLabel(48422921,reg)
		else
			tc:RegisterFlagEffect(48422921,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1,att)
		end
	end
end
function c48422921.descon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetBattleTarget()
	return tp==e:GetOwnerPlayer() and tc and tc:IsControler(1-tp) and tc:IsAttribute(e:GetLabel())
end
function c48422921.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetBattleTarget()
	Duel.Destroy(tc,REASON_EFFECT)
end
