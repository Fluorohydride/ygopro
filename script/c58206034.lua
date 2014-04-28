--漆黒の魔王 LV8
function c58206034.initial_effect(c)
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c58206034.discon)
	e1:SetOperation(c58206034.disop)
	c:RegisterEffect(e1)
	--redirect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_BATTLE_DESTROY_REDIRECT)
	e2:SetCondition(c58206034.discon)
	e2:SetValue(LOCATION_REMOVED)
	c:RegisterEffect(e2)
end
c58206034.lvupcount=1
c58206034.lvup={12817939}
c58206034.lvdncount=2
c58206034.lvdn={85313220,12817939}
function c58206034.discon(e)
	return e:GetHandler():GetFlagEffect(12817939)~=0
end
function c58206034.disop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local d=Duel.GetAttackTarget()
	if d==c then d=Duel.GetAttacker() end
	if d and d:IsStatus(STATUS_BATTLE_DESTROYED) and d:IsType(TYPE_EFFECT) and not c:IsStatus(STATUS_BATTLE_DESTROYED) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x17a0000)
		d:RegisterEffect(e1)
	end
end
