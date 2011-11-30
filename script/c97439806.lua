--魔轟神クシャノ
function c97439806.initial_effect(c)
	--to hand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(97439806,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c97439806.cost)
	e1:SetTarget(c97439806.tg)
	e1:SetOperation(c97439806.op)
	c:RegisterEffect(e1)
end
function c97439806.costfilter(c)
	return c:IsSetCard(0x35) and c:GetCode()~=97439806 and c:IsDiscardable()
end
function c97439806.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c97439806.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c97439806.costfilter,1,1,REASON_DISCARD+REASON_COST)
end
function c97439806.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToHand() end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,e:GetHandler(),1,0,0)
end
function c97439806.op(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SendtoHand(e:GetHandler(),nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,e:GetHandler())
	end
end
