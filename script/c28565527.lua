--黄龍の召喚士
function c28565527.initial_effect(c)
	--tohand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(28565527,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1,28565527)
	e1:SetCost(c28565527.cost)
	e1:SetTarget(c28565527.target)
	e1:SetOperation(c28565527.operation)
	c:RegisterEffect(e1)
end
function c28565527.cfilter(c)
	return Duel.IsExistingTarget(Card.IsAbleToHand,0,LOCATION_MZONE,LOCATION_MZONE,1,c)
end
function c28565527.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c28565527.cfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c28565527.cfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c28565527.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsAbleToHand() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c28565527.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
