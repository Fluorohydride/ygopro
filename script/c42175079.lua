--大噴火
function c42175079.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_END_PHASE,0)
	e1:SetCondition(c42175079.condition)
	e1:SetTarget(c42175079.target)
	e1:SetOperation(c42175079.activate)
	c:RegisterEffect(e1)
end
function c42175079.filter(c)
	return c:IsFaceup() and c:IsCode(10080320)
end
function c42175079.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_END and Duel.GetTurnPlayer()==tp
		and Duel.IsExistingMatchingCard(c42175079.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c42175079.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c42175079.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
	Duel.Destroy(g,REASON_EFFECT)
end
