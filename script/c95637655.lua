--バックアップ・ウォリアー
function c95637655.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c95637655.spcon)
	c:RegisterEffect(e1)
	--spsummon cost
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_COST)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCost(c95637655.spcost)
	e2:SetOperation(c95637655.spop)
	c:RegisterEffect(e2)
	Duel.AddCustomActivityCounter(95637655,ACTIVITY_SPSUMMON,c95637655.counterfilter)
end
function c95637655.counterfilter(c)
	return c:GetSummonType()~=SUMMON_TYPE_SYNCHRO
end
function c95637655.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==2
		and not Duel.IsExistingMatchingCard(Card.IsAttackPos,tp,LOCATION_MZONE,0,1,nil)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
end
function c95637655.spcost(e,c,tp)
	return Duel.GetCustomActivityCount(95637655,tp,ACTIVITY_SPSUMMON)==0
end
function c95637655.spop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c95637655.splimit)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c95637655.splimit(e,c,tp,sumtp,sumpos)
	return bit.band(sumtp,SUMMON_TYPE_SYNCHRO)==SUMMON_TYPE_SYNCHRO
end
