--六武衆の露払い
function c78792195.initial_effect(c)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(78792195,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c78792195.con)
	e2:SetCost(c78792195.cost)
	e2:SetTarget(c78792195.target)
	e2:SetOperation(c78792195.operation)
	c:RegisterEffect(e2)
end
function c78792195.confilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c78792195.con(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c78792195.confilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c78792195.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsSetCard,1,nil,0x3d) end
	local sg=Duel.SelectReleaseGroup(tp,Card.IsSetCard,1,1,nil,0x3d)
	Duel.Release(sg,REASON_COST)
end
function c78792195.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c78792195.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
