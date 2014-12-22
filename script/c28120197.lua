--断層地帯
function c28120197.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage amp
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e2:SetCondition(c28120197.dcon)
	e2:SetOperation(c28120197.dop)
	c:RegisterEffect(e2)
end
function c28120197.dcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsDefencePos() and eg:GetFirst():IsRace(RACE_ROCK)
end
function c28120197.dop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,ev*2)
end
