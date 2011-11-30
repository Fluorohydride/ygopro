--ナチュル·ハイドランジー
function c47795344.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c47795344.spcon)
	c:RegisterEffect(e1)
	if not c47795344.global_check then
		c47795344.global_check=true
		c47795344[0]=false
		c47795344[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CHAIN_SOLVED)
		ge1:SetOperation(c47795344.chop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c47795344.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c47795344.spcon(e,c)
	if c==nil then return true end
	return c47795344[c:GetControler()] and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c47795344.chop(e,tp,eg,ep,ev,re,r,rp)
	local rc=re:GetHandler()
	if rc:IsSetCard(0x2a) and rc:IsType(TYPE_MONSTER)
		and Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)==LOCATION_MZONE then
		c47795344[rp]=true
	end
end
function c47795344.clear(e,tp,eg,ep,ev,re,r,rp)
	c47795344[0]=false
	c47795344[1]=false
end
