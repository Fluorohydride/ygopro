--水霊使いエリア
function c74364659.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(74364659,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c74364659.target)
	e1:SetOperation(c74364659.operation)
	c:RegisterEffect(e1)
end
function c74364659.filter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER) and c:IsControlerCanBeChanged()
end
function c74364659.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c74364659.filter(chkc) end
	if chk==0 then return true end
	if not e:GetHandler():IsStatus(STATUS_BATTLE_DESTROYED) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
		local g=Duel.SelectTarget(tp,c74364659.filter,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,g:GetCount(),0,0)
	end
end
function c74364659.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc and tc:IsRelateToEffect(e) and c74364659.filter(tc) then
		c:CreateRelation(tc,RESET_EVENT+0x5fe0000)
		local e1=Effect.CreateEffect(c)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_CONTROL)
		e1:SetValue(tp)
		e1:SetLabel(0)
		e1:SetReset(RESET_EVENT+0x1fc0000)
		e1:SetCondition(c74364659.ctcon)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_CHANGE_POS)
		e2:SetReset(RESET_EVENT+0x1fc0000)
		e2:SetOperation(c74364659.posop)
		e2:SetLabelObject(e1)
		tc:RegisterEffect(e2)
	end
end
function c74364659.ctcon(e)
	if e:GetLabel()==1 then return true end
	if e:GetLabel()==2 then return false end
	local c=e:GetOwner()
	local h=e:GetHandler()
	if h:IsAttribute(ATTRIBUTE_WATER) and not c:IsDisabled() and c:IsRelateToCard(h) then
		return true
	else
		e:SetLabel(2)
		return false
	end
end
function c74364659.posop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabelObject():GetLabel()~=0 then return end
	local h=e:GetHandler()
	if h:IsPreviousPosition(POS_FACEUP) and h:IsFacedown() then
		e:GetLabelObject():SetLabel(1)
	end
end
