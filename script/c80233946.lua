--ゴラ・タートル
function c80233946.initial_effect(c)
	--cannot attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c80233946.atktarget)
	c:RegisterEffect(e1)
end
function c80233946.atktarget(e,c)
	return c:IsAttackAbove(1900)
end
