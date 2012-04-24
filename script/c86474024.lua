--共同戦線
function c86474024.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c86474024.condition)
	e1:SetTarget(c86474024.target)
	e1:SetOperation(c86474024.activate)
	c:RegisterEffect(e1)
end
function c86474024.filter1(c,tp)
	local lv1=c:GetLevel()
	return lv1>0 and c:IsFaceup() and Duel.IsExistingMatchingCard(c86474024.filter2,tp,LOCATION_MZONE,0,1,c,lv1)
end
function c86474024.filter2(c,lv1)
	return c:IsFaceup() and c:GetLevel()==lv1
end
function c86474024.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsActiveType(TYPE_TRAP) and re:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.IsChainNegatable(ev)
		and Duel.IsExistingMatchingCard(c86474024.filter1,tp,LOCATION_MZONE,0,1,nil,tp)
end
function c86474024.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsDestructable() and re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c86474024.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
