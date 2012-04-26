--ヒーロー見参
function c21597117.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c21597117.condition)
	e1:SetTarget(c21597117.target)
	e1:SetOperation(c21597117.activate)
	c:RegisterEffect(e1)
end
function c21597117.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c21597117.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c21597117.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c21597117.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
end
function c21597117.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local sg=g:RandomSelect(1-tp,1)
	local tc=sg:GetFirst()
	if tc then
		Duel.ConfirmCards(1-tp,tc)
		if tc:IsCanBeSpecialSummoned(e,0,tp,false,false) then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		else
			Duel.SendtoGrave(tc,REASON_EFFECT)
		end
	end
end
