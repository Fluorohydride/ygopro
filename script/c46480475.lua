--不協和音
function c46480475.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetTarget(c46480475.target)
	c:RegisterEffect(e1)
	--disable spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,1)
	e2:SetTarget(c46480475.splimit)
	c:RegisterEffect(e2)
end
function c46480475.splimit(e,c,tp,sumtp,sumpos)
	return bit.band(sumtp,SUMMON_TYPE_SYNCHRO)==SUMMON_TYPE_SYNCHRO
end
function c46480475.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetLabel(3)
	e1:SetCountLimit(1)
	e1:SetCondition(c46480475.tgcon)
	e1:SetOperation(c46480475.tgop)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END+RESET_SELF_TURN,3)
	e:GetHandler():RegisterEffect(e1)
end
function c46480475.tgcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c46480475.tgop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	ct=ct-1
	e:SetLabel(ct)
	if ct==0 then
		Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
	end
end
