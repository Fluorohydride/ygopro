--ドドドボット
function c19700943.initial_effect(c)
	--summon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_LIMIT_SUMMON_PROC)
	e1:SetCondition(c19700943.sumcon)
	c:RegisterEffect(e1)
	--immune
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetOperation(c19700943.atkop)
	c:RegisterEffect(e2)
end
function c19700943.sumcon(e,c)
	if not c then return true end
	return false
end
function c19700943.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_IMMUNE_EFFECT)
	e1:SetValue(c19700943.efilter)
	e1:SetReset(RESET_PHASE+PHASE_DAMAGE)
	c:RegisterEffect(e1)
end
function c19700943.efilter(e,te)
	return te:GetOwner()~=e:GetOwner()
end
