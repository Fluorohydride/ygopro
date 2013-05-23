--平和の使者
function c44656491.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--cannot attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c44656491.atktarget)
	c:RegisterEffect(e2)
	--maintain
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetOperation(c44656491.mtop)
	c:RegisterEffect(e3)
end
function c44656491.atktarget(e,c)
	return c:GetAttack()>=1500
end
function c44656491.mtop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	if Duel.GetLP(tp)>100 and Duel.SelectYesNo(tp,aux.Stringid(44656491,0)) then
		Duel.PayLPCost(tp,100)
	else
		Duel.Destroy(e:GetHandler(),REASON_RULE)
	end
end
