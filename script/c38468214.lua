--エーリアン・ヒュプノ
function c38468214.initial_effect(c)
	aux.EnableDualAttribute(c)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(38468214,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(aux.IsDualState)
	e1:SetTarget(c38468214.target)
	e1:SetOperation(c38468214.operation)
	c:RegisterEffect(e1)
end
function c38468214.filter(c)
	return c:GetCounter(0xe)>0 and c:IsControlerCanBeChanged()
end
function c38468214.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c38468214.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c38468214.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c38468214.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c38468214.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:GetCounter(0xe)>0 and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_CONTROL)
		e1:SetValue(tp)
		e1:SetReset(RESET_EVENT+0x1fc0000)
		e1:SetCondition(c38468214.ctcon)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PHASE+PHASE_END)
		e2:SetRange(LOCATION_MZONE)
		e2:SetCountLimit(1)
		e2:SetLabel(tp)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetCondition(c38468214.rmctcon)
		e2:SetOperation(c38468214.rmctop)
		tc:RegisterEffect(e2)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_SINGLE)
		e3:SetCode(EFFECT_SELF_DESTROY)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		e3:SetCondition(c38468214.descon)
		tc:RegisterEffect(e3)
	end
end
function c38468214.ctcon(e)
	local c=e:GetOwner()
	return c:IsHasCardTarget(e:GetHandler()) and not c:IsDisabled()
end
function c38468214.rmctcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==e:GetLabel()
end
function c38468214.rmctop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RemoveCounter(tp,0xe,1,REASON_EFFECT)
end
function c38468214.descon(e)
	return e:GetHandler():GetCounter(0xe)==0
end
