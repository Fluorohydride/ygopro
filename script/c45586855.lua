--A・O・J サイクロン・クリエイター
function c45586855.initial_effect(c)
	--to hand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(45586855,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c45586855.cost)
	e1:SetTarget(c45586855.target)
	e1:SetOperation(c45586855.operation)
	c:RegisterEffect(e1)
end
function c45586855.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD,nil)
end
function c45586855.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TUNER)
end
function c45586855.rfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToHand()
end
function c45586855.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=Duel.GetMatchingGroupCount(c45586855.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if chk==0 then return Duel.IsExistingMatchingCard(c45586855.rfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,ct,nil) end
	local rg=Duel.GetMatchingGroup(c45586855.rfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,rg,ct,0,0)
end
function c45586855.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=Duel.GetMatchingGroupCount(c45586855.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local rg=Duel.GetMatchingGroup(c45586855.rfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	if rg:GetCount()<ct then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local sg=rg:Select(tp,ct,ct,nil)
	Duel.HintSelection(sg)
	Duel.SendtoHand(sg,nil,REASON_EFFECT)
end
