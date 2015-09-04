--地霊使いアウス
function c37970940.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(37970940,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c37970940.target)
	e1:SetOperation(c37970940.operation)
	c:RegisterEffect(e1)
end
function c37970940.filter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_EARTH) and c:IsControlerCanBeChanged()
end
function c37970940.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c37970940.filter(chkc) end
	if chk==0 then return true end
	if not e:GetHandler():IsStatus(STATUS_BATTLE_DESTROYED) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
		local g=Duel.SelectTarget(tp,c37970940.filter,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,g:GetCount(),0,0)
	end
end
function c37970940.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc and tc:IsRelateToEffect(e) and c37970940.filter(tc) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_CONTROL)
		e1:SetProperty(EFFECT_FLAG_OWNER_RELATE+EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetValue(tp)
		e1:SetLabel(0)
		e1:SetReset(RESET_EVENT+0x1fc0000)
		e1:SetCondition(c37970940.ctcon)
		tc:RegisterEffect(e1)
	end
end
function c37970940.ctcon(e)
	local c=e:GetOwner()
	local h=e:GetHandler()
	return h:IsAttribute(ATTRIBUTE_EARTH) and c:IsHasCardTarget(h)
end
