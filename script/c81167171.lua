--ヒーロースピリッツ
function c81167171.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c81167171.condition)
	e1:SetOperation(c81167171.activate)
	c:RegisterEffect(e1)
	if not c81167171.global_check then
		c81167171.global_check=true
		c81167171[0]=false
		c81167171[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DESTROY)
		ge1:SetOperation(c81167171.checkop1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c81167171.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c81167171.checkop1(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsLocation(LOCATION_MZONE) and tc:IsSetCard(0x3008) and tc:IsReason(REASON_BATTLE) then
			c81167171[tc:GetControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c81167171.clear(e,tp,eg,ep,ev,re,r,rp)
	c81167171[0]=false
	c81167171[1]=false
end
function c81167171.condition(e,tp,eg,ep,ev,re,r,rp)
	return c81167171[tp] and Duel.GetCurrentPhase()==PHASE_BATTLE
end
function c81167171.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_CALCULATING)
	e1:SetReset(RESET_PHASE+PHASE_DAMAGE_CAL)
	e1:SetOperation(c81167171.batop)
	Duel.RegisterEffect(e1,tp)
end
function c81167171.batop(e,tp,eg,ep,ev,re,r,rp)
	local s=Duel.GetAttacker()
	local o=Duel.GetAttackTarget()
	if Duel.GetTurnPlayer()~=tp then
		s=Duel.GetAttackTarget()
		o=Duel.GetAttacker()
	end
	if not o then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_NO_BATTLE_DAMAGE)
	e1:SetReset(RESET_PHASE+PHASE_DAMAGE_CAL)
	o:RegisterEffect(e1)
	if s then
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e2:SetReset(RESET_PHASE+PHASE_DAMAGE_CAL)
		e2:SetValue(1)
		s:RegisterEffect(e2)
	end
	e:Reset()
end
