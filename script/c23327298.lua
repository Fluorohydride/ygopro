--パラレル・セレクト
function c23327298.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c23327298.condition)
	e1:SetTarget(c23327298.target)
	e1:SetOperation(c23327298.operation)
	c:RegisterEffect(e1)
end
function c23327298.cfilter(c,tp)
	return c:IsType(TYPE_SYNCHRO) and c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsPreviousPosition(POS_FACEUP)
		and c:IsControler(tp) and c:GetPreviousControler()==tp and c:GetReasonPlayer()~=tp
end
function c23327298.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c23327298.cfilter,1,nil,tp)
end
function c23327298.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL) and c:IsAbleToHand()
end
function c23327298.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_REMOVED) and chkc:IsControler(tp) and c23327298.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c23327298.filter,tp,LOCATION_REMOVED,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c23327298.filter,tp,LOCATION_REMOVED,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c23327298.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
