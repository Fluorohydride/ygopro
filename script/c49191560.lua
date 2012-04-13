--魔導剣士 シャリオ
function c49191560.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49191560,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c49191560.cost)
	e1:SetTarget(c49191560.target)
	e1:SetOperation(c49191560.operation)
	c:RegisterEffect(e1)
end
function c49191560.cfilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL) and c:IsDiscardable()
end
function c49191560.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c49191560.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c49191560.cfilter,1,1,REASON_DISCARD+REASON_COST)
end
function c49191560.filter(c)
	return c:IsRace(RACE_SPELLCASTER) and c:IsAbleToHand()
end
function c49191560.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c49191560.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c49191560.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c49191560.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c49191560.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
