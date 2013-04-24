--飛翔するG
function c80978111.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(80978111,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c80978111.condition)
	e1:SetTarget(c80978111.target)
	e1:SetOperation(c80978111.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(1,0)
	e3:SetTarget(c80978111.splimit)
	c:RegisterEffect(e3)
end
function c80978111.cfilter(c,tp)
	return c:GetSummonPlayer()==tp
end
function c80978111.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c80978111.cfilter,1,nil,1-tp)
end
function c80978111.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(1-tp,LOCATION_MZONE,tp)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c80978111.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.GetLocationCount(1-tp,LOCATION_MZONE,tp)>0 then
		Duel.SpecialSummon(c,0,tp,1-tp,false,false,POS_FACEUP_DEFENCE)
	end
end
function c80978111.splimit(e,c,tp,sumtp,sumpos)
	return bit.band(sumtp,SUMMON_TYPE_XYZ)==SUMMON_TYPE_XYZ
end
