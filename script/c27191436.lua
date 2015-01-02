--バースト・リターン
function c27191436.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c27191436.condition)
	e1:SetTarget(c27191436.target)
	e1:SetOperation(c27191436.activate)
	c:RegisterEffect(e1)
end
function c27191436.cfilter(c)
	return c:IsFaceup() and c:IsCode(58932615)
end
function c27191436.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c27191436.cfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c27191436.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x3008) and not c:IsCode(58932615) and c:IsAbleToHand()
end
function c27191436.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c27191436.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c27191436.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c27191436.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c27191436.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SendtoHand(g,nil,REASON_EFFECT)
end
