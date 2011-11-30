--因果切断
function c71587526.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCost(c71587526.cost)
	e1:SetTarget(c71587526.target)
	e1:SetOperation(c71587526.activate)
	c:RegisterEffect(e1)
end
function c71587526.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c71587526.filter(c)
	return c:IsFaceup() and c:IsAbleToRemove()
end
function c71587526.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) and c71587526.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c71587526.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c71587526.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c71587526.rfilter(c,code)
	return c:GetCode()==code and c:IsAbleToRemove()
end
function c71587526.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 then
		local rg=Duel.GetMatchingGroup(c71587526.rfilter,tp,0,LOCATION_GRAVE,nil,tc:GetCode())
		Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)
	end
end
