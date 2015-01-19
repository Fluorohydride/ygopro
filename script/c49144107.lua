--デス・ウサギ
function c49144107.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49144107,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c49144107.target)
	e1:SetOperation(c49144107.operation)
	c:RegisterEffect(e1)
end
function c49144107.filter(c)
	local tpe=c:GetType()
	return c:IsFaceup() and bit.band(tpe,TYPE_NORMAL)~=0 and bit.band(tpe,TYPE_TOKEN)==0
end
function c49144107.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	local dam=Duel.GetMatchingGroupCount(c49144107.filter,tp,LOCATION_MZONE,0,nil)*1000
	Duel.SetTargetParam(dam)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
end
function c49144107.operation(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local dam=Duel.GetMatchingGroupCount(c49144107.filter,tp,LOCATION_MZONE,0,nil)*1000
	Duel.Damage(p,dam,REASON_EFFECT)
end
