--疾風！凶殺陣
function c71934924.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage cal
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_BATTLED)
	e2:SetOperation(c71934924.atop)
	c:RegisterEffect(e2)
	--atkup
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_DAMAGE_STEP_END)
	e3:SetOperation(c71934924.upop)
	c:RegisterEffect(e3)
end
function c71934924.check(c,tp)
	return c and c:IsSetCard(0x3d) and c:IsControler(tp)
end
function c71934924.atop(e,tp,eg,ep,ev,re,r,rp)
	if c71934924.check(Duel.GetAttacker(),tp) or c71934924.check(Duel.GetAttackTarget(),tp) then
		e:GetHandler():RegisterFlagEffect(71934924,RESET_PHASE+PHASE_DAMAGE,0,1)
	end
end
function c71934924.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d) and c:GetFlagEffect(71934924)==0
end
function c71934924.upop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetFlagEffect(71934924)==0 then return end
	local g=Duel.GetMatchingGroup(c71934924.filter,tp,LOCATION_MZONE,0,nil)
	local tc=g:GetFirst()
	while tc do
		--Activate
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(300)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		tc:RegisterFlagEffect(71934924,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		tc=g:GetNext()
	end
end
