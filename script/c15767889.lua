--騎士デイ・グレファー
function c15767889.initial_effect(c)
	aux.EnableDualAttribute(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15767889,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,15767889)
	e1:SetCondition(c15767889.thcon)
	e1:SetTarget(c15767889.thtg)
	e1:SetOperation(c15767889.thop)
	c:RegisterEffect(e1)
end
function c15767889.thcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsDualState() and Duel.GetTurnPlayer()==tp
end
function c15767889.filter(c)
	return c:IsType(TYPE_EQUIP) and c:IsAbleToHand()
end
function c15767889.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c15767889.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c15767889.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c15767889.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c15767889.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
