--王宮の弾圧
function c93016201.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c93016201.cost1)
	e1:SetTarget(c93016201.target1)
	e1:SetOperation(c93016201.activate1)
	c:RegisterEffect(e1)
	--Activate(timing)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(93016201,0))
	e2:SetType(EFFECT_TYPE_ACTIVATE)
	e2:SetCode(EVENT_SPSUMMON)
	e2:SetCondition(c93016201.condition2)
	e2:SetCost(c93016201.cost2)
	e2:SetTarget(c93016201.target2)
	e2:SetOperation(c93016201.activate2)
	c:RegisterEffect(e2)
	--instant
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(93016201,0))
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_BOTH_SIDE)
	e3:SetCode(EVENT_SPSUMMON)
	e3:SetCondition(c93016201.condition2)
	e3:SetCost(c93016201.cost2)
	e3:SetTarget(c93016201.target2)
	e3:SetOperation(c93016201.activate2)
	c:RegisterEffect(e3)
	--instant(chain)
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(93016201,0))
	e4:SetType(EFFECT_TYPE_QUICK_O)
	e4:SetRange(LOCATION_SZONE)
	e4:SetProperty(EFFECT_FLAG_BOTH_SIDE)
	e4:SetCode(EVENT_CHAINING)
	e4:SetCondition(c93016201.condition3)
	e4:SetCost(c93016201.cost3)
	e4:SetTarget(c93016201.target3)
	e4:SetOperation(c93016201.activate3)
	c:RegisterEffect(e4)
end
function c93016201.cost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	if not Duel.CheckLPCost(tp,800) then return end
	local ct=Duel.GetCurrentChain()
	if ct==1 then return end
	local pe=Duel.GetChainInfo(ct-1,CHAININFO_TRIGGERING_EFFECT)
	if not pe:IsHasCategory(CATEGORY_SPECIAL_SUMMON) then return end
	if not Duel.IsChainDisablable(ct-1) then return end
	if Duel.SelectYesNo(tp,aux.Stringid(93016201,1)) then
		Duel.PayLPCost(tp,800)
		e:SetLabel(1)
	end
end
function c93016201.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if e:GetLabel()~=1 then return end
	local ct=Duel.GetCurrentChain()
	local te=Duel.GetChainInfo(ct-1,CHAININFO_TRIGGERING_EFFECT)
	local tc=te:GetHandler()
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,tc,1,0,0)
	if tc:IsDestructable() and tc:IsRelateToEffect(te) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,tc,1,0,0)
	end
end
function c93016201.activate1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()~=1 then return end
	if not c:IsRelateToEffect(e) then return end
	local ct=Duel.GetChainInfo(0,CHAININFO_CHAIN_COUNT)
	local te=Duel.GetChainInfo(ct-1,CHAININFO_TRIGGERING_EFFECT)
	local tc=te:GetHandler()
	Duel.NegateEffect(ct-1)
	if tc:IsRelateToEffect(te) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c93016201.condition2(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()==0
end
function c93016201.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
end
function c93016201.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE_SUMMON,eg,eg:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,eg:GetCount(),0,0)
end
function c93016201.activate2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.NegateSummon(eg)
	Duel.Destroy(eg,REASON_EFFECT)
end
function c93016201.condition3(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasCategory(CATEGORY_SPECIAL_SUMMON) and Duel.IsChainDisablable(ev)
end
function c93016201.cost3(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
end
function c93016201.target3(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
	if re:GetHandler():IsDestructable() and re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c93016201.activate3(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.NegateEffect(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
