--フリント
function c75560629.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c75560629.target)
	e1:SetOperation(c75560629.operation)
	c:RegisterEffect(e1)
	--atkdown
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_EQUIP)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetCondition(c75560629.flcon)
	e2:SetValue(-300)
	c:RegisterEffect(e2)
	--cannot attack
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_EQUIP)
	e3:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e3:SetCondition(c75560629.flcon)
	c:RegisterEffect(e3)
	--cannot change pos
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_EQUIP)
	e4:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
	e4:SetCondition(c75560629.flcon)
	c:RegisterEffect(e4)
	--equip limit
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_EQUIP_LIMIT)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e5:SetValue(c75560629.eqlimit)
	c:RegisterEffect(e5)
	--reequip
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(75560629,0))
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e6:SetCode(EVENT_TO_GRAVE)
	e6:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e6:SetCondition(c75560629.eqcon)
	e6:SetTarget(c75560629.target)
	e6:SetOperation(c75560629.operation)
	c:RegisterEffect(e6)
	local e7=e6:Clone()
	e7:SetCode(EVENT_REMOVE)
	c:RegisterEffect(e7)
end
function c75560629.flcon(e)
	local tc=e:GetHandler():GetEquipTarget()
	return tc:GetCode()~=83812099 or tc:IsDisabled()
end
function c75560629.eqlimit(e,c)
	return c:GetCode()~=83812099 or c:IsDisabled()
		or not c:GetEquipGroup():IsExists(Card.IsCode,1,e:GetHandler(),75560629)
end
function c75560629.filter(c)
	return c:IsFaceup() and (c:GetCode()~=83812099 or c:IsDisabled()
		or not c:GetEquipGroup():IsExists(Card.IsCode,1,nil,75560629))
end
function c75560629.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c75560629.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c75560629.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c75560629.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c75560629.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
	end
end
function c75560629.eqcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ec=c:GetPreviousEquipTarget()
	return c:IsReason(REASON_LOST_TARGET) and ec and ec:IsReason(REASON_DESTROY)
end
