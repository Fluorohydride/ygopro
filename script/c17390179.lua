--閃光の騎士
function c17390179.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c17390179.op)
	c:RegisterEffect(e1)
end
function c17390179.op(e,tp,eg,ep,ev,re,r,rp)
end
