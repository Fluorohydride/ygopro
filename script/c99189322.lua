--アルカナコール
function c99189322.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c99189322.target)
	e1:SetOperation(c99189322.activate)
	c:RegisterEffect(e1)
end
function c99189322.filter(c)
	return c:GetFlagEffect(36690018)~=0
end
function c99189322.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c99189322.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c99189322.filter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingMatchingCard(Card.IsSetCard,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil,0x5) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c99189322.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c99189322.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and c99189322.filter(tc) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local regc=Duel.SelectMatchingCard(tp,Card.IsSetCard,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,1,nil,0x5):GetFirst()
		if not regc then return end
		Duel.Remove(regc,POS_FACEUP,REASON_EFFECT)
		local regfun=regc.arcanareg
		if not regfun then return end
		local val=tc:GetFlagEffectLabel(36690018)
		tc:ResetEffect(RESET_DISABLE,RESET_EVENT)
		regfun(tc,val)
		tc:RegisterFlagEffect(99189322,RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END,0,1)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetLabelObject(tc)
		e1:SetOperation(c99189322.rec_effect)
		e1:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(e1,tp)
	end
end
function c99189322.rec_effect(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:GetFlagEffect(99189322)==0 or tc:GetFlagEffect(36690018)==0 then return end
	local regfun=tc.arcanareg
	if not regfun then return end
	local val=tc:GetFlagEffectLabel(36690018)
	tc:ResetEffect(RESET_DISABLE,RESET_EVENT)
	regfun(tc,val)
end
