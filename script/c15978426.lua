--EMセカンドンキー
function c15978426.initial_effect(c)
	--tograve
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15978426,0))
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_SEARCH+CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c15978426.tgtg)
	e1:SetOperation(c15978426.tgop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
end
function c15978426.filter(c,tohand)
	return c:IsSetCard(0x9f) and not c:IsCode(15978426) and c:IsType(TYPE_MONSTER)
		and (c:IsAbleToGrave() or (tohand and c:IsAbleToHand()))
end
function c15978426.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local tohand=Duel.GetFieldCard(tp,LOCATION_SZONE,6) and Duel.GetFieldCard(tp,LOCATION_SZONE,7)
		return Duel.IsExistingMatchingCard(c15978426.filter,tp,LOCATION_DECK,0,1,nil,tohand)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c15978426.tgop(e,tp,eg,ep,ev,re,r,rp)
	local tohand=Duel.GetFieldCard(tp,LOCATION_SZONE,6) and Duel.GetFieldCard(tp,LOCATION_SZONE,7)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c15978426.filter,tp,LOCATION_DECK,0,1,1,nil,tohand)
	local tc=g:GetFirst()
	if not tc then return end
	if tohand and tc:IsAbleToHand() and (not tc:IsAbleToGrave() or Duel.SelectYesNo(tp,aux.Stringid(15978426,1))) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	else
		Duel.SendtoGrave(tc,REASON_EFFECT)
	end
end
