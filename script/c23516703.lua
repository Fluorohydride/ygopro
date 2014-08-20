--サモンリミッター
function c23516703.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--disable summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_SUMMON_COUNT_LIMIT)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,1)
	e2:SetValue(c23516703.sumval)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SPSUMMON_COUNT_LIMIT)
	c:RegisterEffect(e3)
	local e4=e2:Clone()
	e4:SetCode(EFFECT_FLIP_SUMMON_COUNT_LIMIT)
	c:RegisterEffect(e4)
	if not c23516703.global_check then
		c23516703.global_check=true
		c23516703[0]=0
		c23516703[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetOperation(c23516703.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
		ge2:SetOperation(c23516703.checkop)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge3:SetOperation(c23516703.checkop)
		Duel.RegisterEffect(ge3,0)
		local ge4=Effect.CreateEffect(c)
		ge4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge4:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge4:SetOperation(c23516703.clear)
		Duel.RegisterEffect(ge4,0)
	end
end
function c23516703.sumval(e,se,sp)
	return 2-c23516703[sp]
end
function c23516703.checkop(e,tp,eg,ep,ev,re,r,rp)
	local s1=false
	local s2=false
	local tc=eg:GetFirst()
	while tc do
		if tc:GetSummonPlayer()==0 then s1=true
		else s2=true end
		tc=eg:GetNext()
	end
	if s1 then c23516703[0]=c23516703[0]+1 end
	if s2 then c23516703[1]=c23516703[1]+1 end
end
function c23516703.clear(e,tp,eg,ep,ev,re,r,rp)
	c23516703[0]=0
	c23516703[1]=0
end
