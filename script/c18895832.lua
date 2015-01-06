--システム・ダウン
function c18895832.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c18895832.cost)
	e1:SetTarget(c18895832.target)
	e1:SetOperation(c18895832.activate)
	c:RegisterEffect(e1)
end
function c18895832.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c18895832.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_MACHINE)
end
function c18895832.tfilter(c)
	return not c:IsAbleToRemove()
end
function c18895832.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c18895832.filter,tp,0,LOCATION_MZONE+LOCATION_GRAVE,nil)
		return g:GetCount()>0 and not g:IsExists(c18895832.tfilter,1,nil)
	end
	local g=Duel.GetMatchingGroup(c18895832.filter,tp,0,LOCATION_MZONE+LOCATION_GRAVE,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c18895832.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c18895832.filter,tp,0,LOCATION_MZONE+LOCATION_GRAVE,nil)
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
