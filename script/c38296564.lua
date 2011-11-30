--安全地帯
function c38296564.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c38296564.target)
	e1:SetOperation(c38296564.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c38296564.desop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--Destroy2
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c38296564.descon2)
	e3:SetOperation(c38296564.desop2)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
end
function c38296564.filter(c)
	return c:IsFaceup() and c:IsAttack()
end
function c38296564.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c38296564.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c38296564.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUPATTACK)
	local g=Duel.SelectTarget(tp,c38296564.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c38296564.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		e:SetLabelObject(tc)
		c:CreateRelation(tc,RESET_EVENT+0x1fe0000)
		tc:CreateRelation(c,RESET_EVENT+0x1020000)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c38296564.rcon)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
		e2:SetValue(c38296564.efilter)
		tc:RegisterEffect(e2)
		local e3=e2:Clone()
		e3:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		tc:RegisterEffect(e3)
		local e4=e1:Clone()
		e4:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
		e4:SetCondition(c38296564.acon)
		e4:SetLabel(tp)
		tc:RegisterEffect(e4)
	end
end
function c38296564.rcon(e)
	return e:GetOwner():IsRelateToCard(e:GetHandler()) and not e:GetOwner():IsDisabled()
end
function c38296564.acon(e)
	return e:GetOwner():IsRelateToCard(e:GetHandler()) and e:GetHandlerPlayer()==e:GetLabel()
		and not e:GetOwner():IsDisabled()
end
function c38296564.efilter(e,re)
	return e:GetOwnerPlayer()~=re:GetOwnerPlayer()
end
function c38296564.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=e:GetLabelObject():GetLabelObject()
	if not tc or tc:IsStatus(STATUS_DESTROY_CONFIRMED) then return end
	if tc:IsRelateToCard(c) then
		Duel.Destroy(tc, REASON_EFFECT)
	end
end
function c38296564.descon2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_DESTROY_CONFIRMED) then return false end
	local tc=e:GetLabelObject():GetLabelObject()
	return tc and eg:IsContains(tc) and tc:IsRelateToCard(c) and c:IsRelateToCard(tc)
end
function c38296564.desop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(), REASON_EFFECT)
end
