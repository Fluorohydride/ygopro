--士気高揚
function c93671934.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--recover
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_EQUIP)
	e2:SetOperation(c93671934.recop)
	c:RegisterEffect(e2)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c93671934.damcon)
	e3:SetOperation(c93671934.damop)
	c:RegisterEffect(e3)
end
function c93671934.recop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Recover(eg:GetFirst():GetControler(),1000,REASON_EFFECT)
end
function c93671934.filter(c)
	return c:GetEquipTarget()~=nil or c:IsReason(REASON_LOST_TARGET)
end
function c93671934.damcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c93671934.filter,1,nil)
end
function c93671934.damop(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c93671934.filter,nil)
	if g:IsExists(Card.IsControler,1,nil,tp) then
		Duel.Damage(tp,1000,REASON_EFFECT)
	end
	if g:IsExists(Card.IsControler,1,nil,1-tp) then
		Duel.Damage(1-tp,1000,REASON_EFFECT)
	end
end
