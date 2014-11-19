--グリザイユの牢獄
function c22888900.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetCondition(c22888900.condition)
	e1:SetOperation(c22888900.operation)
	c:RegisterEffect(e1)
end
function c22888900.cfilter(c)
	local sumtype=c:GetSummonType()
	return c:IsFaceup() and (bit.band(sumtype,SUMMON_TYPE_ADVANCE)==SUMMON_TYPE_ADVANCE
		or bit.band(sumtype,SUMMON_TYPE_RITUAL)==SUMMON_TYPE_RITUAL
		or bit.band(sumtype,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION)
end
function c22888900.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c22888900.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c22888900.operation(e,tp,eg,ep,ev,re,r,rp)
	--
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c22888900.tg)
	e1:SetCode(EFFECT_DISABLE)
	if Duel.GetTurnPlayer()~=tp then
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,2)
	else
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN)
	end
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_ATTACK)
	Duel.RegisterEffect(e2,tp)
	--
	local e3=Effect.CreateEffect(e:GetHandler())
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(1,1)
	e3:SetTarget(c22888900.splimit)
	if Duel.GetTurnPlayer()~=tp then
		e3:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,2)
	else
		e3:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN)
	end
	Duel.RegisterEffect(e3,tp)
end
function c22888900.tg(e,c)
	return c:IsType(TYPE_SYNCHRO+TYPE_XYZ)
end
function c22888900.splimit(e,c,tp,sumtp,sumpos)
	return bit.band(sumtp,SUMMON_TYPE_SYNCHRO)==SUMMON_TYPE_SYNCHRO or bit.band(sumtp,SUMMON_TYPE_XYZ)==SUMMON_TYPE_XYZ
end
