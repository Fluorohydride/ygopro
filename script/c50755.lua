--マジシャンズ・サークル
function c50755.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c50755.condition)
	e1:SetTarget(c50755.target)
	e1:SetOperation(c50755.activate)
	c:RegisterEffect(e1)
end
function c50755.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttacker():IsRace(RACE_SPELLCASTER)
end
function c50755.filter(c,e,tp)
	return c:IsAttackBelow(2000) and c:IsRace(RACE_SPELLCASTER) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c50755.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c50755.filter,tp,LOCATION_DECK,0,1,nil,e,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c50755.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c50755.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		local tc=g:GetFirst()
		if tc then Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK) end
	end
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(1-tp,c50755.filter,1-tp,LOCATION_DECK,0,1,1,nil,e,1-tp)
		local tc=g:GetFirst()
		if tc then Duel.SpecialSummonStep(tc,0,1-tp,1-tp,false,false,POS_FACEUP_ATTACK) end
	end
	Duel.SpecialSummonComplete()
end
