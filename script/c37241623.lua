--紋章の記録
function c37241623.initial_effect(c)
	Duel.EnableGlobalFlag(GLOBALFLAG_DETACH_EVENT)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c37241623.condition)
	e1:SetTarget(c37241623.target)
	e1:SetOperation(c37241623.activate)
	c:RegisterEffect(e1)
	if not c37241623.global_check then
		c37241623.global_check=true
		c37241623[0]=nil
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DETACH_MATERIAL)
		ge1:SetOperation(c37241623.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c37241623.checkop(e,tp,eg,ep,ev,re,r,rp)
	local cid=Duel.GetCurrentChain()
	if cid>0 then
		c37241623[0]=Duel.GetChainInfo(cid,CHAININFO_CHAIN_ID)
	end
end
function c37241623.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and Duel.GetChainInfo(0,CHAININFO_CHAIN_ID)==c37241623[0] and re:IsActiveType(TYPE_XYZ) and Duel.IsChainNegatable(ev)
end
function c37241623.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c37241623.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.Destroy(re:GetHandler(),REASON_EFFECT)
	end
end
