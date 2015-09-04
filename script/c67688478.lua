--ヂェミナイ・デビル
function c67688478.initial_effect(c)
	--Negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(67688478,0))
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_HAND)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCondition(c67688478.condition)
	e1:SetCost(c67688478.cost)
	e1:SetTarget(c67688478.target)
	e1:SetOperation(c67688478.operation)
	c:RegisterEffect(e1)
end
function c67688478.condition(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp or (not re:IsHasType(EFFECT_TYPE_ACTIVATE) and not re:IsActiveType(TYPE_MONSTER))
		or (not Duel.IsChainNegatable(ev)) then return false end
	local ex,tg,tc,p=Duel.GetOperationInfo(ev,CATEGORY_HANDES)
	return re:IsHasCategory(CATEGORY_HANDES) and (not ex or p~=1-tp)
end
function c67688478.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c67688478.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsDestructable() and re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	end
end
function c67688478.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) and Duel.Destroy(eg,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
