--ナチュル・チェリー
function c60668166.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60668166,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c60668166.condition)
	e1:SetTarget(c60668166.target)
	e1:SetOperation(c60668166.operation)
	c:RegisterEffect(e1)
end
function c60668166.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return rp~=tp and c:GetPreviousControler()==tp and bit.band(c:GetPreviousLocation(),LOCATION_ONFIELD)~=0
end
function c60668166.filter(c,e,tp)
	return c:GetCode()==60668166 and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c60668166.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c60668166.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c60668166.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct==0 then return end
	local g=Duel.GetMatchingGroup(c60668166.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()>0 then
		local t1=g:GetFirst()
		local t2=g:GetNext()
		Duel.SpecialSummonStep(t1,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
		Duel.ConfirmCards(1-tp,t1)
		if t2 and ct>1 and Duel.SelectYesNo(tp,aux.Stringid(60668166,1)) then
			Duel.SpecialSummonStep(t2,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
			Duel.ConfirmCards(1-tp,t2)
		end
		Duel.SpecialSummonComplete()
	end
end
