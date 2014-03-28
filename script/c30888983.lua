--方舟の選別
function c30888983.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE_SUMMON+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON)
	e1:SetCondition(c30888983.condition)
	e1:SetCost(c30888983.cost)
	e1:SetTarget(c30888983.target)
	e1:SetOperation(c30888983.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON)
	c:RegisterEffect(e3)
end
function c30888983.cfilter(c,rc)
	return c:IsFaceup() and c:IsRace(rc)
end
function c30888983.filter(c)
	return Duel.IsExistingMatchingCard(c30888983.cfilter,0,LOCATION_MZONE,LOCATION_MZONE,1,nil,c:GetRace())
end
function c30888983.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()==0 and eg:IsExists(c30888983.filter,1,nil)
end
function c30888983.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c30888983.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=eg:Filter(c30888983.filter,nil)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c30888983.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c30888983.filter,nil)
	Duel.NegateSummon(g)
	Duel.Destroy(g,REASON_EFFECT)
end
