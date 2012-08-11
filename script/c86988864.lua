--みつこぶラクーダ
function c86988864.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(86988864,0))
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c86988864.condition)
	e1:SetCost(c86988864.cost)
	e1:SetTarget(c86988864.target)
	e1:SetOperation(c86988864.operation)
	c:RegisterEffect(e1)
end
function c86988864.cfilter(c)
	return c:IsFaceup() and c:IsCode(86988864)
end
function c86988864.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c86988864.cfilter,tp,LOCATION_MZONE,0,3,nil)
end
function c86988864.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c86988864.cfilter,2,nil) end
	local g=Duel.SelectReleaseGroup(tp,c86988864.cfilter,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c86988864.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,3) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(3)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,3)
end
function c86988864.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
