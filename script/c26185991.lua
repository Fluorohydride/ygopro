--代打バッター
function c26185991.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26185991,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c26185991.condition)
	e1:SetTarget(c26185991.target)
	e1:SetOperation(c26185991.operation)
	c:RegisterEffect(e1)
end
function c26185991.condition(e,tp,eg,ep,ev,re,r,rp)
	local pp=e:GetHandler():GetPreviousControler()
	local pl=e:GetHandler():GetPreviousLocation()
	return pp==tp and (pl==LOCATION_MZONE or pl==LOCATION_SZONE)
end
function c26185991.filter(c,e,sp)
	return c:IsRace(RACE_INSECT) and c:IsCanBeSpecialSummoned(e,0,sp,false,false)
end
function c26185991.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c26185991.filter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c26185991.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c26185991.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
