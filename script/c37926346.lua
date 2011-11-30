--インヴェルズ·ローチ
function c37926346.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterEqualFunction(Card.GetLevel,4),2)
	c:EnableReviveLimit()
	--disable spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE_SUMMON+CATEGORY_DESTROY)
	e1:SetDescription(aux.Stringid(37926346,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_SPSUMMON)
	e1:SetCondition(c37926346.condition)
	e1:SetCost(c37926346.cost)
	e1:SetTarget(c37926346.target)
	e1:SetOperation(c37926346.operation)
	c:RegisterEffect(e1)
end
function c37926346.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():GetLevel()>=5 and Duel.GetCurrentChain()==0
end
function c37926346.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c37926346.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,eg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
end
function c37926346.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	Duel.NegateSummon(eg:GetFirst())
	Duel.Destroy(eg,REASON_EFFECT)
end
