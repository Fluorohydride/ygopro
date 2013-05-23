--六芒星の呪縛
function c18807108.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c18807108.target)
	e1:SetOperation(c18807108.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetCondition(c18807108.descon)
	e2:SetOperation(c18807108.desop)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c18807108.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) end
	if chk==0 then return Duel.IsExistingTarget(nil,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,nil,tp,0,LOCATION_MZONE,1,1,nil)
end
function c18807108.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		c:CreateRelation(tc,RESET_EVENT+0x1fe0000)
		e:GetLabelObject():SetLabelObject(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_OWNER_RELATE)
		e1:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
		e1:SetReset(RESET_EVENT+0x1fc0000)
		e1:SetCondition(c18807108.rcon)
		tc:RegisterEffect(e1,true)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CANNOT_ATTACK)
		tc:RegisterEffect(e2,true)
	end
end
function c18807108.rcon(e)
	return not e:GetHandler():IsImmuneToEffect(e) and e:GetOwner():IsRelateToCard(e:GetHandler())
end
function c18807108.descon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_DESTROY_CONFIRMED) then return false end
	local tc=e:GetLabelObject()
	return tc and eg:IsContains(tc) and tc:IsReason(REASON_DESTROY) and c:IsRelateToCard(tc)
end
function c18807108.desop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
