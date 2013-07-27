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
		e4:SetOperation(c27770341.addcount1)
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
		if c:GetPreviousRaceOnField()==RACE_DRAGON then c27770341[p]=c27770341[p]+1 end
		c=eg:GetNext()
	end
end
function c27770341.addcount1(e,tp,eg,ep,ev,re,r,rp)
	local c=eg:GetFirst()
	while c~=nil do
		local p=c:GetControler()
		if c:GetOriginalRace()==RACE_DRAGON then c27770341[p]=c27770341[p]+1 end
		c=eg:GetNext()
	end
end
function c27770341.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetOperation(c27770341.droperation)
	Duel.RegisterEffect(e1,tp)
end
function c27770341.droperation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_CARD,0,27770341)
	Duel.Draw(tp,c27770341[tp],REASON_EFFECT)
end
