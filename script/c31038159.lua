--創世竜
function c31038159.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(31038159,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c31038159.thcost)
	e1:SetTarget(c31038159.thtg)
	e1:SetOperation(c31038159.thop)
	c:RegisterEffect(e1)
	--return to deck
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(31038159,1))
	e2:SetCategory(CATEGORY_TODECK)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c31038159.tdcon)
	e2:SetTarget(c31038159.tdtg)
	e2:SetOperation(c31038159.tdop)
	c:RegisterEffect(e2)
end
function c31038159.cfilter(c)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToGraveAsCost()
end
function c31038159.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c31038159.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c31038159.cfilter,1,1,REASON_COST)
end
function c31038159.thfilter(c)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToHand()
end
function c31038159.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c31038159.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c31038159.thfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c31038159.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c31038159.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
function c31038159.tdcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
end
function c31038159.tdfilter(c)
	return c:IsRace(RACE_DRAGON) and c:IsAbleToDeck()
end
function c31038159.tdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c31038159.tdfilter,tp,LOCATION_GRAVE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c31038159.tdfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c31038159.tdop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c31038159.tdfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
