--炎塵爆発
function c38049934.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCost(c38049934.cost)
	e1:SetTarget(c38049934.target)
	e1:SetOperation(c38049934.activate)
	c:RegisterEffect(e1)
end
c38049934.check=false
function c38049934.cfilter(c)
	return c:IsSetCard(0x39) and c:IsAbleToRemoveAsCost()
end
function c38049934.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	c38049934.check=true
	if chk==0 then return Duel.IsExistingMatchingCard(c38049934.cfilter,tp,LOCATION_GRAVE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c38049934.cfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
	e:SetLabel(g:GetCount())
end
function c38049934.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not c38049934.check then return false end
		c38049934.check=false
		return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler())
	end
	c38049934.check=false
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c38049934.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectMatchingCard(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetLabel(),e:GetHandler())
	Duel.Destroy(g,REASON_EFFECT)
end
