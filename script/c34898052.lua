--아룡포효
function c34898052.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(34898052,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCost(c34898052.cost)
	e1:SetTarget(c34898052.target1)
	e1:SetOperation(c34898052.activate1)
	c:RegisterEffect(e1)
end
function c34898052.rfilter(c,att)
	return c:IsAttribute(att) and c:IsType(TYPE_MONSTER) and c:IsAbleToRemoveAsCost()
end
function c34898052.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=e:GetLabel()
	if chk==0 then return
			Duel.IsExistingMatchingCard(c34898052.rfilter,tp,LOCATION_GRAVE,0,1,nil,ATTRIBUTE_EARTH)
		and Duel.IsExistingMatchingCard(c34898052.rfilter,tp,LOCATION_GRAVE,0,1,nil,ATTRIBUTE_WATER)
		and Duel.IsExistingMatchingCard(c34898052.rfilter,tp,LOCATION_GRAVE,0,1,nil,ATTRIBUTE_FIRE)
		and Duel.IsExistingMatchingCard(c34898052.rfilter,tp,LOCATION_GRAVE,0,1,nil,ATTRIBUTE_WIND)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c34898052.rfilter,tp,LOCATION_GRAVE,0,1,1,nil,ATTRIBUTE_EARTH)
	local g1=Duel.SelectMatchingCard(tp,c34898052.rfilter,tp,LOCATION_GRAVE,0,1,1,nil,ATTRIBUTE_WATER)
	local g2=Duel.SelectMatchingCard(tp,c34898052.rfilter,tp,LOCATION_GRAVE,0,1,1,nil,ATTRIBUTE_FIRE)
	local g3=Duel.SelectMatchingCard(tp,c34898052.rfilter,tp,LOCATION_GRAVE,0,1,1,nil,ATTRIBUTE_WIND)
	g:Merge(g1)	
	g:Merge(g2)	
	g:Merge(g3)			
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c34898052.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToDeck,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	local g=Duel.GetMatchingGroup(Card.IsAbleToDeck,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c34898052.activate1(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectMatchingCard(tp,Card.IsAbleToDeck,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
	if g:GetCount()>0 then
		Duel.HintSelection(g)
		Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	end
end