--究極恐獣
function c15894048.initial_effect(c)
	--attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_FIRST_ATTACK)
	e1:SetCondition(c15894048.facon)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MUST_ATTACK)
	e2:SetCondition(c15894048.facon)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_ATTACK_ALL)
	e3:SetCondition(c15894048.facon)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	if not c15894048.global_check then
		c15894048.global_check=true
		c15894048[0]=0
		c15894048[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_ATTACK_ANNOUNCE)
		ge1:SetOperation(c15894048.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_BATTLE)
		ge2:SetOperation(c15894048.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c15894048.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if c15894048[tc:GetControler()]==0 then
		c15894048[tc:GetControler()]=1
		tc:RegisterFlagEffect(15894048,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE,0,1)
	elseif tc:GetFlagEffect(15894048)==0 then
		c15894048[tc:GetControler()]=2
	end
end
function c15894048.clear(e,tp,eg,ep,ev,re,r,rp)
	c15894048[0]=0
	c15894048[1]=0
end
function c15894048.facon(e)
	local tp=e:GetHandlerPlayer()
	return Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0
		and c15894048[tp]==e:GetHandler():GetFlagEffect(15894048)
end
