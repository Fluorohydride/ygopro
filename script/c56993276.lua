--エレキャンセル
function c56993276.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE_SUMMON+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON)
	e1:SetCondition(c56993276.condition)
	e1:SetCost(c56993276.cost)
	e1:SetTarget(c56993276.target)
	e1:SetOperation(c56993276.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON)
	c:RegisterEffect(e2)
end
function c56993276.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(1-tp) and Duel.GetCurrentChain()==0
end
function c56993276.cfilter(c)
	return c:IsSetCard(0xe) and c:IsType(TYPE_MONSTER) and c:IsDiscardable()
end
function c56993276.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c56993276.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c56993276.cfilter,1,1,REASON_COST+REASON_DISCARD,nil)
end
function c56993276.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,eg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
end
function c56993276.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateSummon(eg:GetFirst())
	Duel.Destroy(eg,REASON_EFFECT)
end
