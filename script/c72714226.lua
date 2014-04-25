--アサルト・ガンドッグ
function c72714226.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(72714226,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c72714226.condition)
	e1:SetTarget(c72714226.target)
	e1:SetOperation(c72714226.operation)
	c:RegisterEffect(e1)
end
function c72714226.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsReason(REASON_BATTLE)
end
function c72714226.filter(c,e,tp)
	return c:GetCode()==72714226 and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP)
end
function c72714226.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c72714226.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c72714226.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct==0 then return end
	local g=Duel.GetMatchingGroup(c72714226.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()>0 then
		local t1=g:GetFirst()
		local t2=g:GetNext()
		Duel.SpecialSummonStep(t1,0,tp,tp,false,false,POS_FACEUP)
		Duel.ConfirmCards(1-tp,t1)
		if t2 and ct>1 and Duel.SelectYesNo(tp,aux.Stringid(72714226,1)) then
			Duel.SpecialSummonStep(t2,0,tp,tp,false,false,POS_FACEUP)
			Duel.ConfirmCards(1-tp,t2)
		end
		Duel.SpecialSummonComplete()
	end
end
