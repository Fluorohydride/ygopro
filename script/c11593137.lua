--混沌の落とし穴
function c11593137.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE_SUMMON+CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON)
	e1:SetCondition(c11593137.condition)
	e1:SetCost(c11593137.cost)
	e1:SetTarget(c11593137.target)
	e1:SetOperation(c11593137.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON)
	c:RegisterEffect(e3)
end
function c11593137.filter(c)
	return c:IsAttribute(0x30) and c:IsAbleToRemove()
end
function c11593137.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()==0 and eg:IsExists(c11593137.filter,1,nil)
end
function c11593137.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,2000) end
	Duel.PayLPCost(tp,2000)
end
function c11593137.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=eg:Filter(c11593137.filter,nil)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c11593137.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c11593137.filter,nil)
	Duel.NegateSummon(g)
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
