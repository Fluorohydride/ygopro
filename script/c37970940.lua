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
	if chkc then return chkc:GetLocation()==LOCATION_MZONE and chkc:GetControler()~=tp and c37970940.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c37970940.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,g:GetCount(),0,0)
end
function c37970940.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc and tc:IsRelateToEffect(e) and tc:IsControlerCanBeChanged() then
		c:CreateRelation(tc,RESET_EVENT+0x1fe0000)
		tc:CreateRelation(c,RESET_EVENT+0x1fe0000)
		local e1=Effect.CreateEffect(c)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_CONTROL)
		e1:SetValue(tp)
		e1:SetReset(RESET_EVENT+0x1fc0000)
		e1:SetCondition(c37970940.con)
		tc:RegisterEffect(e1)
	end
end
function c37970940.con(e)
	local c=e:GetOwner()
	local h=e:GetHandler()
	if not h:IsAttribute(ATTRIBUTE_EARTH) then return false end
	return not h:IsRelateToCard(c) or (not c:IsDisabled() and c:IsRelateToCard(h))
end
