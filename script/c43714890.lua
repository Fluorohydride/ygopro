--人投げトロール
function c43714890.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(43714890,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c43714890.cost)
	e1:SetTarget(c43714890.target)
	e1:SetOperation(c43714890.operation)
	c:RegisterEffect(e1)
end
function c43714890.cfilter(c)
	local tp=c:GetType()
	return bit.band(tp,TYPE_NORMAL)~=0 and bit.band(tp,TYPE_TOKEN)==0
end
function c43714890.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c43714890.cfilter,1,nil) end
	local sg=Duel.SelectReleaseGroup(tp,c43714890.cfilter,1,1,nil)
	Duel.Release(sg,REASON_COST)
end
function c43714890.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(800)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
end
function c43714890.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
