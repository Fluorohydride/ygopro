--おジャマッスル
function c98259197.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c98259197.target)
	e1:SetOperation(c98259197.activate)
	c:RegisterEffect(e1)
end
function c98259197.filter(c)
	return c:IsFaceup() and c:IsCode(90140980)
		and Duel.IsExistingMatchingCard(c98259197.filter2,0,LOCATION_MZONE,LOCATION_MZONE,1,c)
end
function c98259197.filter2(c)
	return c:IsFaceup() and c:IsSetCard(0xf)
end
function c98259197.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c98259197.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c98259197.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c98259197.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	local dg=Duel.GetMatchingGroup(c98259197.filter2,0,LOCATION_MZONE,LOCATION_MZONE,g:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c98259197.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local dg=Duel.GetMatchingGroup(c98259197.filter2,0,LOCATION_MZONE,LOCATION_MZONE,tc)
	local ct=Duel.Destroy(dg,REASON_EFFECT)
	if ct>0 and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(ct*1000)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
	end
end
