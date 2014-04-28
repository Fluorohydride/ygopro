--ドドドドライバー
function c85310252.initial_effect(c)
	--lv change
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c85310252.spcon)
	e1:SetOperation(c85310252.spop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(85310252,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(2)
	e2:SetCondition(c85310252.condition)
	e2:SetTarget(c85310252.target)
	e2:SetOperation(c85310252.operation)
	c:RegisterEffect(e2)
end
function c85310252.spcon(e,tp,eg,ep,ev,re,r,rp)
	return re and re:GetHandler():IsSetCard(0x82)
end
function c85310252.spop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(85310252,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c85310252.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(85310252)>0
end
function c85310252.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x82) and c:IsLevelAbove(1)
end
function c85310252.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c85310252.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c85310252.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c85310252.filter,tp,LOCATION_MZONE,0,1,1,nil)
	local tc=g:GetFirst()
	local op=0
	if tc:GetLevel()==1 then op=Duel.SelectOption(tp,aux.Stringid(85310252,1))
	else op=Duel.SelectOption(tp,aux.Stringid(85310252,1),aux.Stringid(85310252,2)) end
	e:SetLabel(op)
end
function c85310252.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		if e:GetLabel()==0 then
			e1:SetValue(1)
		else e1:SetValue(-1) end
		tc:RegisterEffect(e1)
	end
end
