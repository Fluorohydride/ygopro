--群雄割拠
function c90846359.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE+0x1c0)
	e1:SetTarget(c90846359.acttg)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c90846359.adjustop)
	c:RegisterEffect(e2)
	--cannot summon,spsummon,flipsummon
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetTargetRange(1,1)
	e4:SetTarget(c90846359.sumlimit)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCode(EFFECT_CANNOT_SUMMON)
	e5:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e5:SetTargetRange(1,1)
	e5:SetTarget(c90846359.sumlimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD)
	e6:SetRange(LOCATION_SZONE)
	e6:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	e6:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e6:SetTargetRange(1,1)
	e6:SetTarget(c90846359.sumlimit)
	c:RegisterEffect(e6)
end
c90846359[0]=0
c90846359[1]=0
function c90846359.acttg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	c90846359[0]=0
	c90846359[1]=0
end
function c90846359.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	if sumpos and bit.band(sumpos,POS_FACEDOWN)>0 then return false end
	local rc=c90846359[sump]
	if targetp then rc=c90846359[targetp] end
	if rc==0 then return false end
	return c:GetRace()~=rc
end
function c90846359.getrace(g)
	local arc=0
	local tc=g:GetFirst()
	while tc do
		arc=bit.bor(arc,tc:GetRace())
		tc=g:GetNext()
	end
	return arc
end
function c90846359.rmfilter(c,rc)
	return c:GetRace()==rc
end
function c90846359.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase==PHASE_DAMAGE and not Duel.IsDamageCalculated()) or phase==PHASE_DAMAGE_CAL then return end
	local g1=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,nil)
	local g2=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	local c=e:GetHandler()
	if g1:GetCount()==0 then c90846359[tp]=0
	else
		local rac=c90846359.getrace(g1)
		if bit.band(rac,rac-1)~=0 then
			if c90846359[tp]==0 or bit.band(c90846359[tp],rac)==0 then
				Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(90846359,0))
				rac=Duel.AnnounceRace(tp,1,rac)
			else rac=c90846359[tp] end
		end
		g1:Remove(c90846359.rmfilter,nil,rac)
		c90846359[tp]=rac
	end
	if g2:GetCount()==0 then c90846359[1-tp]=0
	else
		local rac=c90846359.getrace(g2)
		if bit.band(rac,rac-1)~=0 then
			if c90846359[1-tp]==0 or bit.band(c90846359[1-tp],rac)==0 then
				Duel.Hint(HINT_SELECTMSG,1-tp,aux.Stringid(90846359,0))
				rac=Duel.AnnounceRace(1-tp,1,rac)
			else rac=c90846359[1-tp] end
		end
		g2:Remove(c90846359.rmfilter,nil,rac)
		c90846359[1-tp]=rac
	end
	g1:Merge(g2)
	if g1:GetCount()>0 then
		Duel.SendtoGrave(g1,REASON_EFFECT)
		Duel.Readjust()
	end
end
