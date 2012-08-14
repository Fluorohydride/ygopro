--素早いマンボウ
function c2843014.initial_effect(c)
	--battle destroyed
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(2843014,0))
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c2843014.condition)
	e1:SetTarget(c2843014.target)
	e1:SetOperation(c2843014.operation)
	c:RegisterEffect(e1)
end
function c2843014.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c2843014.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c2843014.filter1(c)
	return c:IsRace(RACE_FISH) and c:IsAbleToGrave()
end
function c2843014.filter2(c,e,tp)
	return c:IsCode(2843014) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c2843014.operation(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c2843014.filter1,tp,LOCATION_DECK,0,1,1,nil)
	if Duel.SendtoGrave(g,REASON_EFFECT)~=0 then
		if ft<=0 then return end
		local tc=Duel.GetFirstMatchingCard(c2843014.filter2,tp,LOCATION_DECK,0,nil,e,tp)
		if tc and Duel.SelectYesNo(tp,aux.Stringid(2843014,1)) then
			Duel.BreakEffect()
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
