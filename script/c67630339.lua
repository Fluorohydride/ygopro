--コンフュージョン・チャフ
function c67630339.initial_effect(c)
	--damage cal
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c67630339.condition)
	e1:SetOperation(c67630339.operation)
	c:RegisterEffect(e1)
	if not c67630339.global_check then
		c67630339.global_check=true
		c67630339[0]=0
		c67630339[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_ATTACK_ANNOUNCE)
		ge1:SetOperation(c67630339.check)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_ATTACK_DISABLED)
		ge2:SetOperation(c67630339.check2)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge3:SetOperation(c67630339.clear)
		Duel.RegisterEffect(ge3,0)
	end
end
function c67630339.check(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if Duel.GetAttackTarget()==nil then
		c67630339[1-tc:GetControler()]=c67630339[1-tc:GetControler()]+1
		Duel.GetAttacker():RegisterFlagEffect(67630339,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		if c67630339[1-tc:GetControler()]==1 then
			c67630339[2]=Duel.GetAttacker()
		end
	end
end
function c67630339.check2(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:GetFlagEffect(67630339)~=0 and Duel.GetAttackTarget()~=nil then
		c67630339[1-tc:GetControler()]=c67630339[1-tc:GetControler()]-1
	end
end
function c67630339.clear(e,tp,eg,ep,ev,re,r,rp)
	c67630339[0]=0
	c67630339[1]=0
end
function c67630339.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetAttackTarget()==nil and c67630339[tp]==2
		and c67630339[2]:GetFlagEffect(67630339)~=0 and Duel.GetAttacker()~=c67630339[2]
end
function c67630339.operation(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=c67630339[2]
	if a:GetFlagEffect(67630339)~=0 and d:GetFlagEffect(67630339)~=0 
		and a:IsAttackable() and not a:IsImmuneToEffect(e) and not d:IsImmuneToEffect(e) then
		Duel.CalculateDamage(a,d)
	end
end
