--超再生能力
function c27770341.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_END_PHASE)
	e1:SetOperation(c27770341.activate)
	c:RegisterEffect(e1)
	if c27770341.counter==nil then
		c27770341.counter=true
		c27770341[0]=0
		c27770341[1]=0
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		e2:SetOperation(c27770341.resetcount)
		Duel.RegisterEffect(e2,0)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e3:SetCode(EVENT_RELEASE)
		e3:SetOperation(c27770341.addcount)
		Duel.RegisterEffect(e3,0)
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
		e4:SetCode(EVENT_DISCARD)
		e4:SetOperation(c27770341.addcount)
		Duel.RegisterEffect(e4,0)
	end
end
function c27770341.resetcount(e,tp,eg,ep,ev,re,r,rp)
	c27770341[0]=0
	c27770341[1]=0
end
function c27770341.addcount(e,tp,eg,ep,ev,re,r,rp)
	local c=eg:GetFirst()
	while c~=nil do
		local p=c:GetControler()
		if c:IsRace(RACE_DRAGON) then c27770341[p]=c27770341[p]+1 end
		c=eg:GetNext()
	end
end
function c27770341.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(7)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetRange(0xff)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetTarget(c27770341.drtarget)
	e1:SetOperation(c27770341.droperation)
	c:RegisterEffect(e1)
end
function c27770341.drtarget(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(c27770341[tp])
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,c27770341[tp])
end
function c27770341.droperation(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	Duel.Draw(p,c27770341[tp],REASON_EFFECT)
end
