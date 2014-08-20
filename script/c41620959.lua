--竜の霊廟
function c41620959.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,41620959+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c41620959.target)
	e1:SetOperation(c41620959.activate)
	c:RegisterEffect(e1)
end
function c41620959.tgfilter(c)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToGrave()
end
function c41620959.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c41620959.tgfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c41620959.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c41620959.tgfilter,tp,LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	if tc and Duel.SendtoGrave(tc,REASON_EFFECT)>0 and tc:IsLocation(LOCATION_GRAVE) and tc:IsRace(RACE_DRAGON) and tc:IsType(TYPE_NORMAL)
		and Duel.IsExistingMatchingCard(c41620959.tgfilter,tp,LOCATION_DECK,0,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(41620959,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g1=Duel.SelectMatchingCard(tp,c41620959.tgfilter,tp,LOCATION_DECK,0,1,1,nil)
		Duel.SendtoGrave(g1,REASON_EFFECT)
	end
end
