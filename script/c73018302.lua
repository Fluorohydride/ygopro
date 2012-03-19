--黒羽を狩る者
function c73018302.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(73018302,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c73018302.descon)
	e1:SetCost(c73018302.descost)
	e1:SetTarget(c73018302.destg)
	e1:SetOperation(c73018302.desop)
	c:RegisterEffect(e1)
end
function c73018302.check(tp)
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()<2 then return false end
	local rac=g:GetFirst():GetRace()
	local tc=g:GetNext()
	while tc do
		if tc:GetRace()~=rac then return false end
		tc=g:GetNext()
	end
	return true
end
function c73018302.descon(e,tp,eg,ep,ev,re,r,rp)
	return c73018302.check(tp)
end
function c73018302.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsAbleToGraveAsCost,1,1,REASON_COST)
end
function c73018302.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c73018302.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) and c73018302.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c73018302.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c73018302.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c73018302.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and c73018302.check(tp) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
