--御前試合
function c53334471.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c53334471.adjustop)
	c:RegisterEffect(e2)
	--adjust
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c53334471.adjustop2)
	c:RegisterEffect(e3)
	--cannot summon,spsummon,flipsummon
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetTargetRange(1,1)
	e4:SetTarget(c53334471.sumlimit)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCode(EFFECT_CANNOT_SUMMON)
	e5:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e5:SetTargetRange(1,1)
	e5:SetTarget(c53334471.sumlimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD)
	e6:SetRange(LOCATION_SZONE)
	e6:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	e6:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e6:SetTargetRange(1,1)
	e6:SetTarget(c53334471.sumlimit)
	c:RegisterEffect(e6)
end
c53334471[0]=0
c53334471[1]=0
function c53334471.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	if sumpos and bit.band(sumpos,POS_FACEDOWN)>0 then return false end
	local at=c53334471[sump]
	if targetp then at=c53334471[targetp] end
	if at==0 then return false end
	return c:IsAttribute(0xff-at)
end
function c53334471.getattribute(g)
	local aat=0
	local tc=g:GetFirst()
	while tc do
		aat=bit.bor(aat,tc:GetAttribute())
		tc=g:GetNext()
	end
	return aat
end
function c53334471.rmfilter(c,at)
	return not c:IsAttribute(0xff-at)
end
function c53334471.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase==PHASE_DAMAGE and not Duel.IsDamageCalculated()) or phase==PHASE_DAMAGE_CAL then return end
	local g1=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,nil)
	local g2=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	local c=e:GetHandler()
	if g1:GetCount()==0 then c53334471[tp]=0
	else
		if c53334471[tp]==0 then
			Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(53334471,0))
			c53334471[tp]=Duel.AnnounceAttribute(tp,1,c53334471.getattribute(g1))
		end
		g1:Remove(c53334471.rmfilter,nil,c53334471[tp])
	end
	if g2:GetCount()==0 then c53334471[1-tp]=0
	else
		if c53334471[1-tp]==0 then
			Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(53334471,0))
			c53334471[1-tp]=Duel.AnnounceAttribute(1-tp,1,c53334471.getattribute(g2))
		end
		g2:Remove(c53334471.rmfilter,nil,c53334471[1-tp])
	end
	g1:Merge(g2)
	if g1:GetCount()>0 then
		Duel.SendtoGrave(g1,REASON_EFFECT)
		Duel.Readjust()
	end
end
function c53334471.op2filter(c)
	return c:IsFaceup() and c:GetAttribute()>0
end
function c53334471.adjustop2(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c53334471.op2filter,tp,LOCATION_MZONE,0,1,nil) then c53334471[tp]=0 end
	if not Duel.IsExistingMatchingCard(c53334471.op2filter,tp,0,LOCATION_MZONE,1,nil) then c53334471[1-tp]=0 end
end
