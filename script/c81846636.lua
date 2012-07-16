--ジェムナイト・ラズリー
function c81846636.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(81846636,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c81846636.condition)
	e1:SetTarget(c81846636.target)
	e1:SetOperation(c81846636.operation)
	c:RegisterEffect(e1)
end
function c81846636.condition(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_RETURN)==0 and bit.band(r,REASON_EFFECT)~=0
end
function c81846636.filter(c)
	return c:IsType(TYPE_NORMAL) and c:IsAbleToHand()
end
function c81846636.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c81846636.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c81846636.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c81846636.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c81846636.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
