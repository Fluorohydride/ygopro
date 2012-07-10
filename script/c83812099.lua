--フリントロック
function c83812099.initial_effect(c)
	--equip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(83812099,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c83812099.eqcon1)
	e1:SetTarget(c83812099.eqtg1)
	e1:SetOperation(c83812099.eqop1)
	c:RegisterEffect(e1)
	--equip
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(83812099,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c83812099.eqcon2)
	e2:SetTarget(c83812099.eqtg2)
	e2:SetOperation(c83812099.eqop2)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e3:SetCondition(c83812099.eqcon2)
	e3:SetValue(1)
	c:RegisterEffect(e3)
end
function c83812099.eqcon1(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():GetEquipGroup():IsExists(Card.IsCode,1,nil,75560629)
end
function c83812099.filter1(c,ec)
	return c:IsFaceup() and c:IsCode(75560629) and c:CheckEquipTarget(ec)
end
function c83812099.eqtg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c83812099.filter1,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil,e:GetHandler()) end
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,nil,1,0,LOCATION_SZONE)
end
function c83812099.eqop1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectMatchingCard(tp,c83812099.filter1,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil,c)
	local eqc=g:GetFirst()
	if eqc and c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.Equip(tp,eqc,c)
	end
end
function c83812099.eqcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetEquipGroup():IsExists(Card.IsCode,1,nil,75560629)
end
function c83812099.filter2(c,eqc)
	return c:IsFaceup() and eqc:CheckEquipTarget(c)
end
function c83812099.eqtg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local eqc=e:GetHandler():GetEquipGroup():Filter(Card.IsCode,nil,75560629):GetFirst()
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c83812099.filter2(chkc,eqc) end
	if chk==0 then return Duel.IsExistingTarget(c83812099.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,e:GetHandler(),eqc) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c83812099.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,e:GetHandler(),eqc)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,eqc,1,0,0)
end
function c83812099.eqop2(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local eqc=e:GetHandler():GetEquipGroup():Filter(Card.IsCode,nil,75560629):GetFirst()
	if eqc and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,eqc,tc)
	end
end
