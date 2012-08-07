--甲虫装機の魔弓 ゼクトアロー
function c87047074.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c87047074.target)
	e1:SetOperation(c87047074.operation)
	c:RegisterEffect(e1)
	--Atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_EQUIP)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(500)
	c:RegisterEffect(e2)
	--Equip limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_EQUIP_LIMIT)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetValue(c87047074.eqlimit)
	c:RegisterEffect(e3)
	--chain limit
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_CHAINING)
	e4:SetCondition(c87047074.chcon)
	e4:SetOperation(c87047074.chop)
	c:RegisterEffect(e4)
end
function c87047074.eqlimit(e,c)
	return c:IsSetCard(0x56)
end
function c87047074.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x56)
end
function c87047074.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c87047074.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c87047074.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c87047074.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c87047074.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
	end
end
function c87047074.chcon(e,tp,eg,ep,ev,re,r,rp)
	return re:GetHandler()==e:GetHandler():GetEquipTarget()
end
function c87047074.chop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SetChainLimit(c87047074.chlimit)
end
function c87047074.chlimit(e,ep,tp)
	return ep==tp
end
