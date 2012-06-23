--エレメンタルバースト
function c61411502.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetCost(c61411502.cost)
	e1:SetTarget(c61411502.target)
	e1:SetOperation(c61411502.activate)
	c:RegisterEffect(e1)
end
function c61411502.cfilter1(c,rg)
	if not c:IsAttribute(ATTRIBUTE_WIND) then return false end
	rg:RemoveCard(c)
	local ret=rg:IsExists(c61411502.cfilter2,1,nil,rg)
	rg:AddCard(c)
	return ret
end
function c61411502.cfilter2(c,rg)
	if not c:IsAttribute(ATTRIBUTE_WATER) then return false end
	rg:RemoveCard(c)
	local ret=rg:IsExists(c61411502.cfilter3,1,nil,rg)
	rg:AddCard(c)
	return ret
end
function c61411502.cfilter3(c,rg)
	if not c:IsAttribute(ATTRIBUTE_FIRE) then return false end
	rg:RemoveCard(c)
	local ret=rg:IsExists(Card.IsAttribute,1,nil,ATTRIBUTE_EARTH)
	rg:AddCard(c)
	return ret
end
function c61411502.rfilter(c)
	return c:IsFaceup() and c:IsHasEffect(EFFECT_EXTRA_RELEASE) and c:IsReleasable()
end
function c61411502.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local g1=Duel.GetMatchingGroup(Card.IsReleasable,tp,LOCATION_MZONE,0,nil)
	local g2=Duel.GetMatchingGroup(c61411502.rfilter,tp,0,LOCATION_MZONE,nil)
	g1:Merge(g2)
	if chk==0 then return g1:GetCount()>3 and g1:IsExists(c61411502.cfilter1,1,nil,g1) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local rg1=g1:FilterSelect(tp,c61411502.cfilter1,1,1,nil,g1)
	g1:Sub(rg1)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local rg2=g1:FilterSelect(tp,c61411502.cfilter2,1,1,nil,g1)
	g1:Sub(rg2)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local rg3=g1:FilterSelect(tp,c61411502.cfilter3,1,1,nil,g1)
	g1:Sub(rg3)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local rg4=g1:FilterSelect(tp,Card.IsAttribute,1,1,nil,ATTRIBUTE_EARTH)
	rg1:Merge(rg2)
	rg1:Merge(rg3)
	rg1:Merge(rg4)
	Duel.Release(rg1,REASON_COST)
end
function c61411502.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c61411502.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
