--独奏の第1楽章
function c44256816.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,44256816+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c44256816.condition)
	e1:SetCost(c44256816.cost)
	e1:SetTarget(c44256816.target)
	e1:SetOperation(c44256816.activate)
	c:RegisterEffect(e1)
	Duel.AddCustomActivityCounter(44256816,ACTIVITY_SPSUMMON,c44256816.counterfilter)
end
function c44256816.counterfilter(c)
	return c:IsSetCard(0x9b)
end
function c44256816.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==0
end
function c44256816.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetCustomActivityCount(44256816,tp,ACTIVITY_SPSUMMON)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetLabelObject(e)
	e1:SetTarget(c44256816.splimit)
	Duel.RegisterEffect(e1,tp)
end
function c44256816.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return not c:IsSetCard(0x9b)
end
function c44256816.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsSetCard(0x9b) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c44256816.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c44256816.filter,tp,LOCATION_DECK+LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,LOCATION_DECK+LOCATION_HAND)
end
function c44256816.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c44256816.filter,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
