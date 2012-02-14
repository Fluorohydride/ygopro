--逆転する運命
function c36690018.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c36690018.target)
	e1:SetOperation(c36690018.activate)
	c:RegisterEffect(e1)
end
function c36690018.filter(c)
	return c:GetFlagEffect(36690018)~=0
end
function c36690018.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c36690018.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c36690018.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c36690018.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c36690018.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and c36690018.filter(tc) then
		local val=tc:GetFlagEffectLabel(36690018)
		tc:SetFlagEffectLabel(36690018,1-val)
	end
end
