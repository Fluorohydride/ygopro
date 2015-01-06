--エンペラー・オーダー
function c35011819.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c35011819.target1)
	e1:SetOperation(c35011819.activate1)
	c:RegisterEffect(e1)
	--instant
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(35011819,0))
	e2:SetCategory(CATEGORY_NEGATE+CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_CHAINING)
	e2:SetCondition(c35011819.condition2)
	e2:SetTarget(c35011819.target2)
	e2:SetOperation(c35011819.activate2)
	c:RegisterEffect(e2)
end
function c35011819.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local ct=Duel.GetCurrentChain()
	if ct==1 then return end
	local ct=Duel.GetCurrentChain()
	local te=Duel.GetChainInfo(ct-1,CHAININFO_TRIGGERING_EFFECT)
	local tc=te:GetHandler()
	if te:GetCode()==EVENT_SUMMON_SUCCESS and te:IsActiveType(TYPE_MONSTER) and Duel.IsChainNegatable(ct-1)
		and Duel.IsPlayerCanDraw(tc:GetControler(),1) and Duel.SelectYesNo(tp,aux.Stringid(35011819,1)) then
		e:SetLabel(1)
		Duel.SetOperationInfo(0,CATEGORY_NEGATE,tc,1,0,0)
		Duel.SetTargetPlayer(tc:GetControler())
		Duel.SetTargetParam(1)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tc:GetControler(),1)
		e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(35011819,2))
	end
end
function c35011819.activate1(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()~=1 then return end
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local ct=Duel.GetChainInfo(0,CHAININFO_CHAIN_COUNT)
	Duel.NegateActivation(ct-1)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c35011819.condition2(e,tp,eg,ep,ev,re,r,rp)
	return re:IsActiveType(TYPE_MONSTER) and re:GetCode()==EVENT_SUMMON_SUCCESS and Duel.IsChainNegatable(ev)
end
function c35011819.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(rp,1) end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	Duel.SetTargetPlayer(rp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,rp,1)
end
function c35011819.activate2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.NegateActivation(ev)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
