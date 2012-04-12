--ワーム·ベイト
function c43140791.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c43140791.condition)
	e1:SetCost(c43140791.cost)
	e1:SetTarget(c43140791.target)
	e1:SetOperation(c43140791.activate)
	c:RegisterEffect(e1)
	if not c43140791.global_check then
		c43140791.global_check=true
		c43140791[0]=true
		c43140791[1]=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetOperation(c43140791.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge2:SetOperation(c43140791.checkop)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge3:SetOperation(c43140791.clear)
		Duel.RegisterEffect(ge3,0)
	end
end
function c43140791.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		local lv=tc:GetLevel()
		if lv==3 or lv==4 then
			c43140791[tc:GetSummonPlayer()]=false
		end
		tc=eg:GetNext()
	end
end
function c43140791.clear(e,tp,eg,ep,ev,re,r,rp)
	c43140791[0]=true
	c43140791[1]=true
end
function c43140791.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_INSECT)
end
function c43140791.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c43140791.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c43140791.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c43140791[tp] end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c43140791.sumlimit)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	Duel.RegisterEffect(e2,tp)
end
function c43140791.sumlimit(e,c,sump,sumtype,sumpos,targetp,se)
	local lv=c:GetLevel()
	return lv==3 or lv==4
end
function c43140791.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsPlayerCanSpecialSummonMonster(tp,43140792,0x3e,0x4011,0,0,1,RACE_INSECT,ATTRIBUTE_EARTH) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,0,0)
end
function c43140791.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>1 
		and Duel.IsPlayerCanSpecialSummonMonster(tp,43140792,0x3e,0x4011,0,0,1,RACE_INSECT,ATTRIBUTE_EARTH) then
		for i=1,2 do
			local token=Duel.CreateToken(tp,43140792)
			Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
		end
		Duel.SpecialSummonComplete()
	end
end
