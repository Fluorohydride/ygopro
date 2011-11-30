--神の宣告
function c41420027.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE_SUMMON+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON)
	e1:SetCondition(c41420027.condition1)
	e1:SetCost(c41420027.cost1)
	e1:SetTarget(c41420027.target1)
	e1:SetOperation(c41420027.activate1)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON)
	c:RegisterEffect(e3)
	--Activate(effect)
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e4:SetType(EFFECT_TYPE_ACTIVATE)
	e4:SetCode(EVENT_CHAINING)
	e4:SetCondition(c41420027.condition2)
	e4:SetCost(c41420027.cost2)
	e4:SetTarget(c41420027.target2)
	e4:SetOperation(c41420027.activate2)
	c:RegisterEffect(e4)
end
function c41420027.condition1(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()==0
end
function c41420027.cost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local lp=Duel.GetLP(tp)
	local costlp=math.floor((lp+1)/2)
	Duel.PayLPCost(tp,costlp)
end
function c41420027.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,eg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
end
function c41420027.activate1(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateSummon(eg:GetFirst())
	Duel.Destroy(eg,REASON_EFFECT)
end
function c41420027.condition2(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.IsChainInactivatable(ev)
end
function c41420027.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local lp=Duel.GetLP(tp)
	local costlp=math.floor((lp+1)/2)
	Duel.PayLPCost(tp,costlp)
end
function c41420027.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if eg:GetFirst():IsDestructable() then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c41420027.activate2(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	if ec:IsRelateToEffect(e) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
