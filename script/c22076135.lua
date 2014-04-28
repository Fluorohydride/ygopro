--水精鱗－アビスタージ
function c22076135.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(22076135,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCountLimit(1,22076135)
	e1:SetCost(c22076135.thcost)
	e1:SetTarget(c22076135.thtg)
	e1:SetOperation(c22076135.thop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
end
function c22076135.cfilter(c)
	return c:IsAttribute(ATTRIBUTE_WATER) and c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c22076135.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c22076135.cfilter,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,c22076135.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c22076135.filter(c)
	return c:IsLevelBelow(3) and c:IsAttribute(ATTRIBUTE_WATER) and c:IsAbleToHand()
end
function c22076135.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c22076135.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c22076135.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c22076135.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c22076135.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
