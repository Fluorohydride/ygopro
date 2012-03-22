--無抵抗の真相
function c83546647.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c83546647.condition)
	e1:SetCost(c83546647.cost)
	e1:SetTarget(c83546647.target)
	e1:SetOperation(c83546647.activate)
	e1:SetLabel(1)
	c:RegisterEffect(e1)
end
function c83546647.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(1-tp) and ep==tp and Duel.GetAttackTarget()==nil
end
function c83546647.spfilter(c,e,tp)
	return c:GetLevel()==1 and not c:IsPublic() and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingMatchingCard(c83546647.spfilter2,tp,LOCATION_DECK,0,1,nil,e,tp,c:GetCode())
end
function c83546647.spfilter2(c,e,tp,code)
	return c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c83546647.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(0)
	return true
end
function c83546647.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=0 then return false end
		e:SetLabel(1)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
			and Duel.IsExistingMatchingCard(c83546647.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local g=Duel.SelectMatchingCard(tp,c83546647.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
	Duel.SetTargetCard(g)
	e:SetLabelObject(g:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_HAND+LOCATION_DECK)
end
function c83546647.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	local tc1=e:GetLabelObject()
	if not tc1:IsRelateToEffect(e) or not tc1:IsCanBeSpecialSummoned(e,0,tp,false,false) then return end
	local tc2=Duel.GetFirstMatchingCard(c83546647.spfilter2,tp,LOCATION_DECK,0,nil,e,tp,tc1:GetCode())
	if tc2 then
		Duel.SpecialSummonStep(tc1,0,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonStep(tc2,0,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonComplete()
	end
end
