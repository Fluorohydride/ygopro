--キャッシュバック
function c59957503.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c59957503.condition)
	e1:SetTarget(c59957503.target)
	e1:SetOperation(c59957503.activate)
	c:RegisterEffect(e1)
	if not c59957503.global_check then
		c59957503.global_check=true
		c59957503[0]=nil
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_PAY_LPCOST)
		ge1:SetOperation(c59957503.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c59957503.checkop(e,tp,eg,ep,ev,re,r,rp)
	c59957503[0]=re
end
function c59957503.condition(e,tp,eg,ep,ev,re,r,rp)
	local acte=c59957503[0]
	c59957503[0]=nil
	return re==acte and rp~=tp and (re:IsActiveType(TYPE_MONSTER) or re:IsHasType(EFFECT_TYPE_ACTIVATE)) and Duel.IsChainNegatable(ev)
end
function c59957503.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_TODECK,eg,1,0,0)
	end
end
function c59957503.activate(e,tp,eg,ep,ev,re,r,rp)
	local ec=re:GetHandler()
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		ec:CancelToGrave()
		Duel.SendtoDeck(ec,nil,2,REASON_EFFECT)
	end
end
