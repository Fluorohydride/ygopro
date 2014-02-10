--マインフィールド
function c24419823.initial_effect(c)
	--salvage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(24419823,0))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetCondition(c24419823.thcon)
	e2:SetTarget(c24419823.thtg)
	e2:SetOperation(c24419823.thop)
	c:RegisterEffect(e2)
end
function c24419823.thcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return not c:IsLocation(LOCATION_DECK) and c:IsPreviousPosition(POS_FACEUP)
end
function c24419823.filter(c)
	return c:IsType(TYPE_FIELD) and c:IsAbleToHand()
end
function c24419823.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c24419823.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c24419823.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c24419823.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c24419823.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
