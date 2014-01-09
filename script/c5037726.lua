--光霊術－「聖」
function c5037726.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c5037726.cost)
	e1:SetTarget(c5037726.target)
	e1:SetOperation(c5037726.operation)
	c:RegisterEffect(e1)
end
function c5037726.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsAttribute,1,nil,ATTRIBUTE_LIGHT) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsAttribute,1,1,nil,ATTRIBUTE_LIGHT)
	Duel.Release(g,REASON_COST)
end
function c5037726.filter(c,e,tp)
	return c:IsFaceup() and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c5037726.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_REMOVED) and c5037726.filter(chkc,e,tp) end
	if chk==0 then
		if e:GetLabel()==1 then
			e:SetLabel(0)
			return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
				and Duel.IsExistingTarget(c5037726.filter,tp,LOCATION_REMOVED,LOCATION_REMOVED,1,nil,e,tp)
		else
			return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
				and Duel.IsExistingTarget(c5037726.filter,tp,LOCATION_REMOVED,LOCATION_REMOVED,1,nil,e,tp)
		end
	end
	e:SetLabel(0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c5037726.filter,tp,LOCATION_REMOVED,LOCATION_REMOVED,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c5037726.cfilter(c)
	return not c:IsPublic() and c:IsType(TYPE_TRAP)
end
function c5037726.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local g=Duel.GetMatchingGroup(c5037726.cfilter,tp,0,LOCATION_HAND,nil)
	if g:GetCount()>0 and Duel.SelectYesNo(1-tp,aux.Stringid(5037726,0)) then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_CONFIRM)
		local sg=g:Select(1-tp,1,1,nil)
		Duel.ConfirmCards(tp,sg)
		Duel.ShuffleHand(1-tp)
		if Duel.IsChainDisablable(0) then
			Duel.NegateEffect(0)
			return
		end
	end
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
