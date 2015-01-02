--ミスター・ボンバー
function c70138455.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(70138455,0))
	e1:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c70138455.condition)
	e1:SetCost(c70138455.cost)
	e1:SetTarget(c70138455.target)
	e1:SetOperation(c70138455.operation)
	c:RegisterEffect(e1)
end
function c70138455.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c70138455.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c70138455.filter(c)
	return c:IsFaceup() and c:IsDestructable() and c:IsAttackBelow(1000)
end
function c70138455.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c70138455.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c70138455.filter,tp,LOCATION_MZONE,LOCATION_MZONE,2,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c70138455.filter,tp,LOCATION_MZONE,LOCATION_MZONE,2,2,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,2,0,0)
end
function c70138455.desfilter(c,e)
	return c:IsRelateToEffect(e) and c:IsFaceup() and c:IsDestructable() and c:IsAttackBelow(1000)
end
function c70138455.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(c70138455.desfilter,nil,e)
	Duel.Destroy(sg,REASON_EFFECT)
end
