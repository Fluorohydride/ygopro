--墓守の大筒持ち
function c99877698.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(99877698,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c99877698.cost)
	e1:SetTarget(c99877698.target)
	e1:SetOperation(c99877698.operation)
	c:RegisterEffect(e1)
end
function c99877698.costfilter(c)
	return c:IsSetCard(0x2e) and c:GetCode()~=99877698
end
function c99877698.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c99877698.costfilter,1,nil) end
	local sg=Duel.SelectReleaseGroup(tp,c99877698.costfilter,1,1,nil)
	Duel.Release(sg,REASON_COST)
end
function c99877698.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(700)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,700)
end
function c99877698.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
