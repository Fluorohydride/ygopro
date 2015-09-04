--エレメント・ソルジャー
function c66712593.initial_effect(c)
	--control
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_CHANGE_CONTROL)
	e1:SetCondition(c66712593.ctlcon)
	c:RegisterEffect(e1)
	--disable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c66712593.discon)
	e2:SetOperation(c66712593.disop)
	c:RegisterEffect(e2)
end
function c66712593.filter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c66712593.ctlcon(e)
	return Duel.IsExistingMatchingCard(c66712593.filter,0,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WATER)
end
function c66712593.discon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and not c:IsStatus(STATUS_BATTLE_DESTROYED)
		and Duel.IsExistingMatchingCard(c66712593.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_EARTH)
end
function c66712593.disop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetReset(RESET_EVENT+0x17a0000)
	bc:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DISABLE_EFFECT)
	e2:SetReset(RESET_EVENT+0x17a0000)
	bc:RegisterEffect(e2)
end
