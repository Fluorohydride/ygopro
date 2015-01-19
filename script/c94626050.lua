--フィッシュ・レイン
function c94626050.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_REMOVE)
	e1:SetCondition(c94626050.condition)
	e1:SetTarget(c94626050.target)
	e1:SetOperation(c94626050.operation)
	c:RegisterEffect(e1)
end
function c94626050.cfilter(c)
	return c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsPreviousPosition(POS_FACEUP)
		and c:IsRace(RACE_FISH+RACE_SEASERPENT+RACE_AQUA)
end
function c94626050.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c94626050.cfilter,1,nil)
end
function c94626050.spfilter(c,e,tp)
	return c:IsLevelBelow(3) and c:IsRace(RACE_FISH+RACE_SEASERPENT+RACE_AQUA) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c94626050.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c94626050.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c94626050.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c94626050.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()~=0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
