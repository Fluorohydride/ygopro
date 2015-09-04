--シャドウ・ダイバー
function c57827484.initial_effect(c)
	aux.EnableDualAttribute(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(57827484,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c57827484.condition)
	e1:SetTarget(c57827484.target)
	e1:SetOperation(c57827484.operation)
	c:RegisterEffect(e1)
end
function c57827484.condition(e,tp,eg,ep,ev,re,r,rp)
	return aux.IsDualState(e) and Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c57827484.filter(c)
	return c:IsFaceup() and c:IsLevelBelow(4) and c:IsAttribute(ATTRIBUTE_DARK) and not c:IsHasEffect(EFFECT_DIRECT_ATTACK)
end
function c57827484.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c57827484.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c57827484.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c57827484.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c57827484.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
