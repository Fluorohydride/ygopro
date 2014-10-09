--宝札雲
function c82760689.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c82760689.activate)
	c:RegisterEffect(e1)
	if not c82760689.global_check then
		c82760689.global_check=true
		c82760689[0]=false
		c82760689[1]=Group.CreateGroup()
		c82760689[1]:KeepAlive()
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetOperation(c82760689.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
		ge2:SetOperation(c82760689.checkop)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge3:SetOperation(c82760689.checkop)
		Duel.RegisterEffect(ge3,0)
		local ge4=Effect.CreateEffect(c)
		ge4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge4:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge4:SetOperation(c82760689.clear)
		Duel.RegisterEffect(ge4,0)
	end
end
function c82760689.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if not c82760689[0] and tc:IsFaceup() and tc:IsSetCard(0x18) then
			if c82760689[1]:IsContains(tc) then c82760689[0]=true
			else c82760689[1]:AddCard(tc) end
		end
		tc=eg:GetNext()
	end
end
function c82760689.clear(e,tp,eg,ep,ev,re,r,rp)
	c82760689[0]=false
	c82760689[1]:Clear()
end
function c82760689.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetCondition(c82760689.drcon)
	e1:SetOperation(c82760689.drop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c82760689.filter(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c82760689.drcon(e,tp,eg,ep,ev,re,r,rp)
	return c82760689[0] or c82760689[1]:IsExists(c82760689.filter,1,nil,c82760689[1])
end
function c82760689.drop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Draw(tp,2,REASON_EFFECT)
end
