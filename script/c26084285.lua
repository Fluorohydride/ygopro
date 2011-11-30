--墓守の監視者
function c26084285.initial_effect(c)
	--Negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26084285,0))
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c26084285.condition)
	e1:SetCost(c26084285.cost)
	e1:SetTarget(c26084285.target)
	e1:SetOperation(c26084285.operation)
	c:RegisterEffect(e1)
end
function c26084285.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ec=eg:GetFirst()
	local te,loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_EFFECT,CHAININFO_TRIGGERING_LOCATION)
	if ep==tp or (not te:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.GetChainInfo(ev,CHAININFO_TYPE)~=TYPE_MONSTER)
		or (not Duel.IsChainInactivatable(ev)) or loc==LOCATION_DECK then return false end
	local ex,tg,tc,p=Duel.GetOperationInfo(ev,CATEGORY_HANDES)
	return ex and (p==ep or p==PLAYER_ALL)
end
function c26084285.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c26084285.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
	local loc=eg:GetFirst():GetLocation()
	if eg:GetFirst():IsDestructable() and loc~=LOCATION_DECK then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c26084285.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	local loc=ec:GetLocation()
	if ec:IsRelateToEffect(e) and loc~=LOCATION_DECK then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
