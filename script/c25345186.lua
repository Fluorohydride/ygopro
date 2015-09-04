--ジェノサイド・ウォー
function c25345186.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c25345186.condition)
	e1:SetOperation(c25345186.activate)
	c:RegisterEffect(e1)
end
function c25345186.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c25345186.activate(e,tp,eg,ep,ev,re,r,rp)
	--destroy
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLED)
	e1:SetOperation(c25345186.regop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e2:SetCountLimit(1)
	e2:SetOperation(c25345186.desop)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
end
function c25345186.regop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttacker()
	tc:RegisterFlagEffect(25345186,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	tc=Duel.GetAttackTarget()
	if tc then
		tc:RegisterFlagEffect(25345186,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end
function c25345186.filter(c)
	return c:GetFlagEffect(25345186)~=0
end
function c25345186.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c25345186.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
