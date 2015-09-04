--パワー・サプライヤー
function c55063681.initial_effect(c)
	--set target
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(55063681,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c55063681.target)
	e1:SetOperation(c55063681.operation)
	c:RegisterEffect(e1)
end
function c55063681.filter(c,ec)
	return c:IsFaceup() and ((ec==c and c:GetFlagEffect(55063681)==0) or (ec~=c and not ec:IsHasCardTarget(c)))
end
function c55063681.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c55063681.filter(chkc,e:GetHandler()) end
	if chk==0 then return Duel.IsExistingTarget(c55063681.filter,tp,LOCATION_MZONE,0,1,nil,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c55063681.filter,tp,LOCATION_MZONE,0,1,1,nil,e:GetHandler())
end
function c55063681.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and c:IsFaceup() and c:IsRelateToEffect(e) then
		if tc~=c then
			c:SetCardTarget(tc)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
			e1:SetRange(LOCATION_MZONE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(400)
			e1:SetCondition(c55063681.atkcon)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
		else
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetRange(LOCATION_MZONE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(400)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
			tc:RegisterFlagEffect(55063681,RESET_EVENT+0x1fe0000,0,1)
		end
	end
end
function c55063681.atkcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
